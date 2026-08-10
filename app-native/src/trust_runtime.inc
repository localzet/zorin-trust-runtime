// ---- Zorin Trust Runtime v0.7: foreground TrustService + visual trust channel ----
// Uses Android Keystore through JNI. A tiny DEX lifecycle shim hosts the long-lived Service;
// all trust protocol, cryptography, policy and visual-channel logic remains native.
// The device identity is an EC P-256 key intentionally usable while the screen is locked so
// cryptographic DEVICE TRUST survives lock. Sensitive owner proofs are still denied while locked.
// Private key material is never exported.
#define ZORIN_TRUST_PORT 47472
#define TRUST_ID_ALIAS "zorin_trust_device_identity_v2"
#define TRUST_PREFS "zorin_trust_runtime"

static volatile int g_trust_state = 0; // 0 offline, 1 pending approval, 2 authenticating, 3 trusted, -1 error
static volatile int g_trust_approve_requested = 0;
static volatile int g_trust_forget_requested = 0;
static char g_trust_status[128] = "WAITING FOR HOST AGENT";
static char g_trust_host_name[128] = "<NONE>";
static char g_trust_host_fp[128] = "<NONE>";
static char g_trust_host_identity[96] = "UNKNOWN";
static char g_trust_host_pub_pending[512];
static char g_trust_host_pub_current[512];
static char g_trust_phone_fp[128] = "INITIALIZING";
static char g_trust_policy[96] = "none";
static volatile unsigned int g_trust_proof_count = 0;
static volatile int g_trust_paired_hint = 0;
static char g_trust_last_proof[128] = "NONE";
static volatile int g_authority_pending = 0;
static volatile int g_authority_decision = 0; /* 0 waiting, 1 approve, -1 deny */
static char g_authority_request_id[96];
static char g_authority_action[256];
static char g_authority_resource[512];
static char g_authority_prompt[384];
static int g_trust_approve_rect[4];
static int g_trust_forget_rect[4];
static int g_trust_thread_started = 0;
static char g_trust_ui_last_frame[5200];

static uint16_t trust_net_u16(uint16_t v) { return (uint16_t)((v << 8) | (v >> 8)); }

static int trust_hex_encode(const uint8_t* in, int n, char* out, int cap) {
    static const char* h="0123456789abcdef";
    if(!in||!out||cap<n*2+1)return -1;
    for(int i=0;i<n;++i){out[i*2]=h[(in[i]>>4)&15];out[i*2+1]=h[in[i]&15];}
    out[n*2]=0; return n*2;
}
static int trust_hexval(char c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;}
static int trust_hex_decode(const char* in,uint8_t* out,int cap){if(!in||!out)return -1;int n=(int)strlen(in);if(n&1||n/2>cap)return -1;for(int i=0;i<n/2;++i){int a=trust_hexval(in[i*2]),b=trust_hexval(in[i*2+1]);if(a<0||b<0)return -1;out[i]=(uint8_t)((a<<4)|b);}return n/2;}

static int trust_attach(JNIEnv** out_env,int* out_attached){
    JavaVM* vm=g_vm?g_vm:(g_activity?g_activity->vm:0);if(!vm)return -1;JNIEnv* env=0;int attached=0;
    if((*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_6)!=JNI_OK){if((*vm)->AttachCurrentThread(vm,(void**)&env,0)!=JNI_OK||!env)return -2;attached=1;}
    *out_env=env;if(out_attached)*out_attached=attached;return 0;
}
static void trust_detach(int attached){JavaVM* vm=g_vm?g_vm:(g_activity?g_activity->vm:0);if(attached&&vm)(*vm)->DetachCurrentThread(vm);}
static int trust_jni_exception(JNIEnv* env){if((*env)->ExceptionCheck(env)){(*env)->ExceptionClear(env);return 1;}return 0;}

static int trust_pref_get(const char* key,char* out,int cap){
    jobject ctx=runtime_context();if(!key||!out||cap<2||!ctx)return -1;out[0]=0;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -2;int rc=-3;
    jclass ac=(*env)->GetObjectClass(env,ctx);jmethodID gp=ac?(*env)->GetMethodID(env,ac,"getSharedPreferences","(Ljava/lang/String;I)Landroid/content/SharedPreferences;"):0;
    jstring pn=(*env)->NewStringUTF(env,TRUST_PREFS);jobject prefs=(gp&&pn)?(*env)->CallObjectMethod(env,ctx,gp,pn,0):0;jclass pc=prefs?(*env)->GetObjectClass(env,prefs):0;
    jmethodID gs=pc?(*env)->GetMethodID(env,pc,"getString","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"):0;jstring jk=(*env)->NewStringUTF(env,key);jstring v=(gs&&jk)?(jstring)(*env)->CallObjectMethod(env,prefs,gs,jk,0):0;
    if(v){const char* s=(*env)->GetStringUTFChars(env,v,0);if(s){int i=0;while(s[i]&&i<cap-1){out[i]=s[i];++i;}out[i]=0;(*env)->ReleaseStringUTFChars(env,v,s);rc=i;}}
    if(trust_jni_exception(env))rc=-4;if(v)(*env)->DeleteLocalRef(env,v);if(jk)(*env)->DeleteLocalRef(env,jk);if(pc)(*env)->DeleteLocalRef(env,pc);if(prefs)(*env)->DeleteLocalRef(env,prefs);if(pn)(*env)->DeleteLocalRef(env,pn);if(ac)(*env)->DeleteLocalRef(env,ac);trust_detach(attached);return rc;
}
static int trust_pref_put(const char* key,const char* value){
    jobject ctx=runtime_context();if(!key||!ctx)return -1;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -2;int rc=-3;
    jclass ac=(*env)->GetObjectClass(env,ctx);jmethodID gp=ac?(*env)->GetMethodID(env,ac,"getSharedPreferences","(Ljava/lang/String;I)Landroid/content/SharedPreferences;"):0;
    jstring pn=(*env)->NewStringUTF(env,TRUST_PREFS);jobject prefs=(gp&&pn)?(*env)->CallObjectMethod(env,ctx,gp,pn,0):0;jclass pc=prefs?(*env)->GetObjectClass(env,prefs):0;
    jmethodID em=pc?(*env)->GetMethodID(env,pc,"edit","()Landroid/content/SharedPreferences$Editor;"):0;jobject ed=em?(*env)->CallObjectMethod(env,prefs,em):0;jclass ec=ed?(*env)->GetObjectClass(env,ed):0;
    jmethodID ps=ec?(*env)->GetMethodID(env,ec,"putString","(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;"):0;jmethodID ap=ec?(*env)->GetMethodID(env,ec,"apply","()V"):0;
    jstring jk=(*env)->NewStringUTF(env,key);jstring jv=(*env)->NewStringUTF(env,value?value:"");if(ps&&ap&&ed&&jk&&jv){jobject e2=(*env)->CallObjectMethod(env,ed,ps,jk,jv);(*env)->CallVoidMethod(env,ed,ap);if(e2)(*env)->DeleteLocalRef(env,e2);rc=0;}
    if(trust_jni_exception(env))rc=-4;if(jv)(*env)->DeleteLocalRef(env,jv);if(jk)(*env)->DeleteLocalRef(env,jk);if(ec)(*env)->DeleteLocalRef(env,ec);if(ed)(*env)->DeleteLocalRef(env,ed);if(pc)(*env)->DeleteLocalRef(env,pc);if(prefs)(*env)->DeleteLocalRef(env,prefs);if(pn)(*env)->DeleteLocalRef(env,pn);if(ac)(*env)->DeleteLocalRef(env,ac);trust_detach(attached);return rc;
}


static int trust_host_list_contains(const char* list,const char* item){
    if(!list||!item||!*item)return 0;int ilen=(int)strlen(item);const char* p=list;
    while(*p){const char* q=p;while(*q&&*q!=';')++q;int n=(int)(q-p);if(n==ilen){int same=1;for(int i=0;i<n;++i)if(p[i]!=item[i]){same=0;break;}if(same)return 1;}p=*q?q+1:q;}
    return 0;
}
static int trust_host_add(const char* hostpub){
    if(!hostpub||!*hostpub)return -1;char list[4096];int n=trust_pref_get("trusted_hosts_v2",list,sizeof(list));if(n<0){list[0]=0;n=0;}if(trust_host_list_contains(list,hostpub))return 0;int hl=(int)strlen(hostpub);if(n+hl+2>=(int)sizeof(list))return -2;if(n>0){list[n++]=';';list[n]=0;}for(int i=0;i<hl;++i)list[n++]=hostpub[i];list[n]=0;return trust_pref_put("trusted_hosts_v2",list);
}
static int trust_host_is_trusted(const char* hostpub){
    char list[4096];if(trust_pref_get("trusted_hosts_v2",list,sizeof(list))>0&&trust_host_list_contains(list,hostpub))return 1;
    char legacy[512];if(trust_pref_get("trusted_host_pub",legacy,sizeof(legacy))>0&&mini_streq(legacy,hostpub)){(void)trust_host_add(hostpub);return 1;}return 0;
}
static int trust_host_remove(const char* hostpub){
    if(!hostpub||!*hostpub)return -1;char list[4096],out[4096];out[0]=0;int on=0;int n=trust_pref_get("trusted_hosts_v2",list,sizeof(list));if(n>0){const char* p=list;while(*p){const char* q=p;while(*q&&*q!=';')++q;int len=(int)(q-p),same=1,hl=(int)strlen(hostpub);if(len!=hl)same=0;else for(int i=0;i<len;++i)if(p[i]!=hostpub[i]){same=0;break;}if(!same&&len>0){if(on>0)out[on++]=';';if(on+len<(int)sizeof(out)){for(int i=0;i<len;++i)out[on++]=p[i];out[on]=0;}}p=*q?q+1:q;}(void)trust_pref_put("trusted_hosts_v2",out);}
    char legacy[512];if(trust_pref_get("trusted_host_pub",legacy,sizeof(legacy))>0&&mini_streq(legacy,hostpub))(void)trust_pref_put("trusted_host_pub","");return 0;
}

static int trust_intent_bool(const char* extra){
    if(!extra||!g_activity||!g_activity->clazz)return 0;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return 0;int yes=0;
    jclass ac=(*env)->GetObjectClass(env,g_activity->clazz);jmethodID gi=ac?(*env)->GetMethodID(env,ac,"getIntent","()Landroid/content/Intent;"):0;jobject intent=gi?(*env)->CallObjectMethod(env,g_activity->clazz,gi):0;jclass ic=intent?(*env)->GetObjectClass(env,intent):0;jmethodID gb=ic?(*env)->GetMethodID(env,ic,"getBooleanExtra","(Ljava/lang/String;Z)Z"):0;jstring key=(*env)->NewStringUTF(env,extra);if(gb&&key&&intent)yes=(*env)->CallBooleanMethod(env,intent,gb,key,JNI_FALSE)?1:0;
    if(trust_jni_exception(env))yes=0;if(key)(*env)->DeleteLocalRef(env,key);if(ic)(*env)->DeleteLocalRef(env,ic);if(intent)(*env)->DeleteLocalRef(env,intent);if(ac)(*env)->DeleteLocalRef(env,ac);trust_detach(attached);return yes;
}
static int trust_intent_autoconnect(void){return trust_intent_bool("dev.zorin.trust.autoconnect");}
static int trust_intent_headless(void){return trust_intent_bool("dev.zorin.trust.headless");}

static int trust_device_locked(void){
    jobject ctx=runtime_context();if(!ctx)return 1;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return 1;int locked=1;
    jclass ac=(*env)->GetObjectClass(env,ctx);jmethodID gs=ac?(*env)->GetMethodID(env,ac,"getSystemService","(Ljava/lang/String;)Ljava/lang/Object;"):0;jstring name=(*env)->NewStringUTF(env,"keyguard");jobject kg=(gs&&name)?(*env)->CallObjectMethod(env,ctx,gs,name):0;jclass kc=kg?(*env)->GetObjectClass(env,kg):0;jmethodID idl=kc?(*env)->GetMethodID(env,kc,"isDeviceLocked","()Z"):0;if(idl)locked=(*env)->CallBooleanMethod(env,kg,idl)?1:0;
    if(trust_jni_exception(env))locked=1;if(kc)(*env)->DeleteLocalRef(env,kc);if(kg)(*env)->DeleteLocalRef(env,kg);if(name)(*env)->DeleteLocalRef(env,name);if(ac)(*env)->DeleteLocalRef(env,ac);trust_detach(attached);return locked;
}

static int trust_random(uint8_t* out,int n){
    if(!out||n<=0)return -1;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -2;int rc=-3;
    jclass c=(*env)->FindClass(env,"java/security/SecureRandom");jmethodID ctor=c?(*env)->GetMethodID(env,c,"<init>","()V"):0;jobject r=ctor?(*env)->NewObject(env,c,ctor):0;jmethodID nb=c?(*env)->GetMethodID(env,c,"nextBytes","([B)V"):0;jbyteArray a=(*env)->NewByteArray(env,n);if(r&&nb&&a){(*env)->CallVoidMethod(env,r,nb,a);(*env)->GetByteArrayRegion(env,a,0,n,(jbyte*)out);rc=n;}
    if(trust_jni_exception(env))rc=-4;if(a)(*env)->DeleteLocalRef(env,a);if(r)(*env)->DeleteLocalRef(env,r);if(c)(*env)->DeleteLocalRef(env,c);trust_detach(attached);return rc;
}

static int trust_fingerprint_der(const uint8_t* der,int derlen,char* out,int cap){
    if(!der||derlen<=0||!out||cap<48)return -1;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -2;int rc=-3;uint8_t d[32];memset(d,0,sizeof(d));
    jclass c=(*env)->FindClass(env,"java/security/MessageDigest");jmethodID gi=c?(*env)->GetStaticMethodID(env,c,"getInstance","(Ljava/lang/String;)Ljava/security/MessageDigest;"):0;jstring alg=(*env)->NewStringUTF(env,"SHA-256");jobject md=(gi&&alg)?(*env)->CallStaticObjectMethod(env,c,gi,alg):0;jmethodID dm=c?(*env)->GetMethodID(env,c,"digest","([B)[B"):0;jbyteArray in=(*env)->NewByteArray(env,derlen);if(in)(*env)->SetByteArrayRegion(env,in,0,derlen,(const jbyte*)der);jbyteArray sum=(dm&&md&&in)?(jbyteArray)(*env)->CallObjectMethod(env,md,dm,in):0;
    if(sum&&(*env)->GetArrayLength(env,sum)>=16){(*env)->GetByteArrayRegion(env,sum,0,16,(jbyte*)d);static const char* h="0123456789ABCDEF";int p=0;for(int i=0;i<16&&p<cap-4;++i){if(i&&!(i%2))out[p++]=':';out[p++]=h[d[i]>>4];out[p++]=h[d[i]&15];}out[p]=0;rc=p;}
    if(trust_jni_exception(env))rc=-4;if(sum)(*env)->DeleteLocalRef(env,sum);if(in)(*env)->DeleteLocalRef(env,in);if(md)(*env)->DeleteLocalRef(env,md);if(alg)(*env)->DeleteLocalRef(env,alg);if(c)(*env)->DeleteLocalRef(env,c);trust_detach(attached);return rc;
}

static jobject trust_open_keystore(JNIEnv* env){
    jclass c=(*env)->FindClass(env,"java/security/KeyStore");if(!c)return 0;jmethodID gi=(*env)->GetStaticMethodID(env,c,"getInstance","(Ljava/lang/String;)Ljava/security/KeyStore;");jstring n=(*env)->NewStringUTF(env,"AndroidKeyStore");jobject ks=(gi&&n)?(*env)->CallStaticObjectMethod(env,c,gi,n):0;jmethodID load=c?(*env)->GetMethodID(env,c,"load","(Ljava/io/InputStream;[C)V"):0;if(ks&&load)(*env)->CallVoidMethod(env,ks,load,0,0);if(n)(*env)->DeleteLocalRef(env,n);if(c)(*env)->DeleteLocalRef(env,c);return ks;
}

static int trust_ensure_identity(void){
    JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -1;int rc=-2;jobject ks=trust_open_keystore(env);if(!ks||trust_jni_exception(env))goto done;
    jclass kc=(*env)->GetObjectClass(env,ks);jmethodID ca=kc?(*env)->GetMethodID(env,kc,"containsAlias","(Ljava/lang/String;)Z"):0;jstring alias=(*env)->NewStringUTF(env,TRUST_ID_ALIAS);jboolean exists=(ca&&alias)?(*env)->CallBooleanMethod(env,ks,ca,alias):0;if(trust_jni_exception(env))goto done;
    if(!exists){
        jclass pgc=(*env)->FindClass(env,"java/security/KeyPairGenerator");jmethodID gi=pgc?(*env)->GetStaticMethodID(env,pgc,"getInstance","(Ljava/lang/String;Ljava/lang/String;)Ljava/security/KeyPairGenerator;"):0;jstring ec=(*env)->NewStringUTF(env,"EC"),provider=(*env)->NewStringUTF(env,"AndroidKeyStore");jobject pg=(gi&&ec&&provider)?(*env)->CallStaticObjectMethod(env,pgc,gi,ec,provider):0;
        jclass bc=(*env)->FindClass(env,"android/security/keystore/KeyGenParameterSpec$Builder");jmethodID ctor=bc?(*env)->GetMethodID(env,bc,"<init>","(Ljava/lang/String;I)V"):0;jobject b=(ctor&&alias)?(*env)->NewObject(env,bc,ctor,alias,12):0;
        jclass sc=(*env)->FindClass(env,"java/lang/String");jobjectArray digs=sc?(*env)->NewObjectArray(env,1,sc,0):0;jstring sha=(*env)->NewStringUTF(env,"SHA-256");if(digs&&sha)(*env)->SetObjectArrayElement(env,digs,0,sha);jmethodID sd=bc?(*env)->GetMethodID(env,bc,"setDigests","([Ljava/lang/String;)Landroid/security/keystore/KeyGenParameterSpec$Builder;"):0;if(sd&&b&&digs){jobject r=(*env)->CallObjectMethod(env,b,sd,digs);if(r)(*env)->DeleteLocalRef(env,r);}
        jclass ecs=(*env)->FindClass(env,"java/security/spec/ECGenParameterSpec");jmethodID ector=ecs?(*env)->GetMethodID(env,ecs,"<init>","(Ljava/lang/String;)V"):0;jstring curve=(*env)->NewStringUTF(env,"secp256r1");jobject espec=(ector&&curve)?(*env)->NewObject(env,ecs,ector,curve):0;jmethodID sap=bc?(*env)->GetMethodID(env,bc,"setAlgorithmParameterSpec","(Ljava/security/spec/AlgorithmParameterSpec;)Landroid/security/keystore/KeyGenParameterSpec$Builder;"):0;if(sap&&b&&espec){jobject r=(*env)->CallObjectMethod(env,b,sap,espec);if(r)(*env)->DeleteLocalRef(env,r);}
        jmethodID build=bc?(*env)->GetMethodID(env,bc,"build","()Landroid/security/keystore/KeyGenParameterSpec;"):0;jobject spec=(build&&b)?(*env)->CallObjectMethod(env,b,build):0;jclass pgoc=pg?(*env)->GetObjectClass(env,pg):0;jmethodID init=pgoc?(*env)->GetMethodID(env,pgoc,"initialize","(Ljava/security/spec/AlgorithmParameterSpec;)V"):0;jmethodID gen=pgoc?(*env)->GetMethodID(env,pgoc,"generateKeyPair","()Ljava/security/KeyPair;"):0;if(init&&gen&&pg&&spec){(*env)->CallVoidMethod(env,pg,init,spec);jobject kp=(*env)->CallObjectMethod(env,pg,gen);if(kp)(*env)->DeleteLocalRef(env,kp);}
        if(spec)(*env)->DeleteLocalRef(env,spec);if(pgoc)(*env)->DeleteLocalRef(env,pgoc);if(espec)(*env)->DeleteLocalRef(env,espec);if(curve)(*env)->DeleteLocalRef(env,curve);if(ecs)(*env)->DeleteLocalRef(env,ecs);if(sha)(*env)->DeleteLocalRef(env,sha);if(digs)(*env)->DeleteLocalRef(env,digs);if(sc)(*env)->DeleteLocalRef(env,sc);if(b)(*env)->DeleteLocalRef(env,b);if(bc)(*env)->DeleteLocalRef(env,bc);if(pg)(*env)->DeleteLocalRef(env,pg);if(provider)(*env)->DeleteLocalRef(env,provider);if(ec)(*env)->DeleteLocalRef(env,ec);if(pgc)(*env)->DeleteLocalRef(env,pgc);
        if(trust_jni_exception(env))goto done;
    }
    rc=0;
done:
    if(alias)(*env)->DeleteLocalRef(env,alias);if(kc)(*env)->DeleteLocalRef(env,kc);if(ks)(*env)->DeleteLocalRef(env,ks);if(trust_jni_exception(env))rc=-4;trust_detach(attached);return rc;
}

static int trust_get_public_der(uint8_t* out,int cap){
    if(trust_ensure_identity()!=0)return -1;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -2;int rc=-3;jobject ks=trust_open_keystore(env);jclass kc=ks?(*env)->GetObjectClass(env,ks):0;jmethodID gc=kc?(*env)->GetMethodID(env,kc,"getCertificate","(Ljava/lang/String;)Ljava/security/cert/Certificate;"):0;jstring alias=(*env)->NewStringUTF(env,TRUST_ID_ALIAS);jobject cert=(gc&&alias)?(*env)->CallObjectMethod(env,ks,gc,alias):0;jclass cc=cert?(*env)->GetObjectClass(env,cert):0;jmethodID gp=cc?(*env)->GetMethodID(env,cc,"getPublicKey","()Ljava/security/PublicKey;"):0;jobject pub=(gp&&cert)?(*env)->CallObjectMethod(env,cert,gp):0;jclass pc=pub?(*env)->GetObjectClass(env,pub):0;jmethodID ge=pc?(*env)->GetMethodID(env,pc,"getEncoded","()[B"):0;jbyteArray der=(ge&&pub)?(jbyteArray)(*env)->CallObjectMethod(env,pub,ge):0;if(der){jsize n=(*env)->GetArrayLength(env,der);if(n<=cap){(*env)->GetByteArrayRegion(env,der,0,n,(jbyte*)out);rc=n;}}
    if(trust_jni_exception(env))rc=-4;if(der)(*env)->DeleteLocalRef(env,der);if(pc)(*env)->DeleteLocalRef(env,pc);if(pub)(*env)->DeleteLocalRef(env,pub);if(cc)(*env)->DeleteLocalRef(env,cc);if(cert)(*env)->DeleteLocalRef(env,cert);if(alias)(*env)->DeleteLocalRef(env,alias);if(kc)(*env)->DeleteLocalRef(env,kc);if(ks)(*env)->DeleteLocalRef(env,ks);trust_detach(attached);return rc;
}

static int trust_sign(const uint8_t* msg,int msglen,uint8_t* sig,int sigcap){
    if(!msg||msglen<=0||!sig||trust_ensure_identity()!=0)return -1;JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -2;int rc=-3;jobject ks=trust_open_keystore(env);jclass kc=ks?(*env)->GetObjectClass(env,ks):0;jmethodID gk=kc?(*env)->GetMethodID(env,kc,"getKey","(Ljava/lang/String;[C)Ljava/security/Key;"):0;jstring alias=(*env)->NewStringUTF(env,TRUST_ID_ALIAS);jobject key=(gk&&alias)?(*env)->CallObjectMethod(env,ks,gk,alias,0):0;
    jclass sc=(*env)->FindClass(env,"java/security/Signature");jmethodID gi=sc?(*env)->GetStaticMethodID(env,sc,"getInstance","(Ljava/lang/String;)Ljava/security/Signature;"):0;jstring alg=(*env)->NewStringUTF(env,"SHA256withECDSA");jobject s=(gi&&alg)?(*env)->CallStaticObjectMethod(env,sc,gi,alg):0;jmethodID is=sc?(*env)->GetMethodID(env,sc,"initSign","(Ljava/security/PrivateKey;)V"):0;jmethodID up=sc?(*env)->GetMethodID(env,sc,"update","([B)V"):0;jmethodID sg=sc?(*env)->GetMethodID(env,sc,"sign","()[B"):0;jbyteArray in=(*env)->NewByteArray(env,msglen);if(in)(*env)->SetByteArrayRegion(env,in,0,msglen,(const jbyte*)msg);if(s&&is&&up&&sg&&key&&in){(*env)->CallVoidMethod(env,s,is,key);(*env)->CallVoidMethod(env,s,up,in);jbyteArray o=(jbyteArray)(*env)->CallObjectMethod(env,s,sg);if(o){jsize n=(*env)->GetArrayLength(env,o);if(n<=sigcap){(*env)->GetByteArrayRegion(env,o,0,n,(jbyte*)sig);rc=n;}(*env)->DeleteLocalRef(env,o);}}
    if(trust_jni_exception(env))rc=-4;if(in)(*env)->DeleteLocalRef(env,in);if(s)(*env)->DeleteLocalRef(env,s);if(alg)(*env)->DeleteLocalRef(env,alg);if(sc)(*env)->DeleteLocalRef(env,sc);if(key)(*env)->DeleteLocalRef(env,key);if(alias)(*env)->DeleteLocalRef(env,alias);if(kc)(*env)->DeleteLocalRef(env,kc);if(ks)(*env)->DeleteLocalRef(env,ks);trust_detach(attached);return rc;
}

static int trust_verify_host(const uint8_t* pubder,int publen,const uint8_t* msg,int msglen,const uint8_t* sig,int siglen){
    JNIEnv* env=0;int attached=0;if(trust_attach(&env,&attached)!=0)return -1;int ok=0;
    jclass kfc=(*env)->FindClass(env,"java/security/KeyFactory");jmethodID kgi=kfc?(*env)->GetStaticMethodID(env,kfc,"getInstance","(Ljava/lang/String;)Ljava/security/KeyFactory;"):0;jstring ec=(*env)->NewStringUTF(env,"EC");jobject kf=(kgi&&ec)?(*env)->CallStaticObjectMethod(env,kfc,kgi,ec):0;jclass xc=(*env)->FindClass(env,"java/security/spec/X509EncodedKeySpec");jmethodID ctor=xc?(*env)->GetMethodID(env,xc,"<init>","([B)V"):0;jbyteArray pba=(*env)->NewByteArray(env,publen);if(pba)(*env)->SetByteArrayRegion(env,pba,0,publen,(const jbyte*)pubder);jobject spec=(ctor&&pba)?(*env)->NewObject(env,xc,ctor,pba):0;jclass kfoc=kf?(*env)->GetObjectClass(env,kf):0;jmethodID gp=kfoc?(*env)->GetMethodID(env,kfoc,"generatePublic","(Ljava/security/spec/KeySpec;)Ljava/security/PublicKey;"):0;jobject pub=(gp&&spec)?(*env)->CallObjectMethod(env,kf,gp,spec):0;
    jclass sc=(*env)->FindClass(env,"java/security/Signature");jmethodID sgi=sc?(*env)->GetStaticMethodID(env,sc,"getInstance","(Ljava/lang/String;)Ljava/security/Signature;"):0;jstring alg=(*env)->NewStringUTF(env,"SHA256withECDSA");jobject so=(sgi&&alg)?(*env)->CallStaticObjectMethod(env,sc,sgi,alg):0;jmethodID iv=sc?(*env)->GetMethodID(env,sc,"initVerify","(Ljava/security/PublicKey;)V"):0;jmethodID up=sc?(*env)->GetMethodID(env,sc,"update","([B)V"):0;jmethodID ve=sc?(*env)->GetMethodID(env,sc,"verify","([B)Z"):0;jbyteArray mba=(*env)->NewByteArray(env,msglen);jbyteArray sba=(*env)->NewByteArray(env,siglen);if(mba)(*env)->SetByteArrayRegion(env,mba,0,msglen,(const jbyte*)msg);if(sba)(*env)->SetByteArrayRegion(env,sba,0,siglen,(const jbyte*)sig);if(so&&iv&&up&&ve&&pub&&mba&&sba){(*env)->CallVoidMethod(env,so,iv,pub);(*env)->CallVoidMethod(env,so,up,mba);ok=(*env)->CallBooleanMethod(env,so,ve,sba)?1:0;}
    if(trust_jni_exception(env))ok=0;if(sba)(*env)->DeleteLocalRef(env,sba);if(mba)(*env)->DeleteLocalRef(env,mba);if(so)(*env)->DeleteLocalRef(env,so);if(alg)(*env)->DeleteLocalRef(env,alg);if(sc)(*env)->DeleteLocalRef(env,sc);if(pub)(*env)->DeleteLocalRef(env,pub);if(kfoc)(*env)->DeleteLocalRef(env,kfoc);if(spec)(*env)->DeleteLocalRef(env,spec);if(pba)(*env)->DeleteLocalRef(env,pba);if(xc)(*env)->DeleteLocalRef(env,xc);if(kf)(*env)->DeleteLocalRef(env,kf);if(ec)(*env)->DeleteLocalRef(env,ec);if(kfc)(*env)->DeleteLocalRef(env,kfc);trust_detach(attached);return ok;
}

static int trust_frame_field(const char* text,const char* key,char* out,int cap){
    if(!text||!key||!out||cap<2)return -1;int kl=(int)strlen(key);const char* p=text;while(*p){const char* line=p;const char* e=p;while(*e&&*e!='\n'&&*e!='\r')++e;if((int)(e-line)>kl&&line[kl]==' '){int match=1;for(int i=0;i<kl;++i)if(line[i]!=key[i]){match=0;break;}if(match){const char* v=line+kl+1;int n=0;while(v<e&&n<cap-1)out[n++]=*v++;out[n]=0;return n;}}while(*e=='\n'||*e=='\r')++e;p=e;}out[0]=0;return -1;
}
static int trust_recv_frame(int fd,char* out,int cap){int pos=0;out[0]=0;while(pos<cap-1){ssize_t n=recv(fd,out+pos,(size_t)(cap-1-pos),0);if(n<=0)return -1;pos+=(int)n;out[pos]=0;if(strstr(out,"\nEND\n")||strstr(out,"\r\nEND\r\n")||(pos>=4&&out[pos-4]=='E'&&out[pos-3]=='N'&&out[pos-2]=='D'))return pos;}return -2;}
static int trust_send_all(int fd,const char* s){int len=(int)strlen(s),pos=0;while(pos<len){ssize_t n=send(fd,s+pos,(size_t)(len-pos),0);if(n<=0)return -1;pos+=(int)n;}return 0;}

// Cross-process UI <-> :trust bridge. NativeActivity and TrustService intentionally live
// in distinct Linux processes, so C globals are NOT shared. Keep pairing approval in a
// tiny app-private file IPC protocol instead of relying on deprecated multi-process
// SharedPreferences semantics. Files live under Context.getFilesDir() (same UID, mode 0600).
static int trust_ui_path(char* out,int cap,const char* leaf){if(!out||cap<8||!leaf||!g_files_dir[0])return -1;int n=snprintf(out,(size_t)cap,"%s/%s",g_files_dir,leaf);return (n>0&&n<cap)?0:-2;}
static int trust_ui_write_file(const char* leaf,const char* data){char path[420];if(trust_ui_path(path,sizeof(path),leaf)!=0||!data)return -1;int fd=open(path,O_WRONLY|O_CREAT|O_TRUNC,0600);if(fd<0)return -2;int len=(int)strlen(data),pos=0;while(pos<len){ssize_t n=write(fd,data+pos,(size_t)(len-pos));if(n<=0){close(fd);return -3;}pos+=(int)n;}close(fd);return 0;}
static int trust_ui_read_file(const char* leaf,char* out,int cap){char path[420];if(trust_ui_path(path,sizeof(path),leaf)!=0)return -1;return read_small(path,out,cap);}
static int trust_ui_publish_state(void){
    if(!g_files_dir[0])return -1;
    char sh[260],nh[260],fph[260],ih[220],pph[260],polh[220],pendingh[1100],lph[260];
    char reqh[220],acth[540],resh[1060],prompth[800],wire[7600],pairedbuf[512];
    int paired=trust_pref_get("trusted_host_pub",pairedbuf,sizeof(pairedbuf))>0;
    trust_hex_encode((const uint8_t*)g_trust_status,(int)strlen(g_trust_status),sh,sizeof(sh));
    trust_hex_encode((const uint8_t*)g_trust_host_name,(int)strlen(g_trust_host_name),nh,sizeof(nh));
    trust_hex_encode((const uint8_t*)g_trust_host_fp,(int)strlen(g_trust_host_fp),fph,sizeof(fph));
    trust_hex_encode((const uint8_t*)g_trust_host_identity,(int)strlen(g_trust_host_identity),ih,sizeof(ih));
    trust_hex_encode((const uint8_t*)g_trust_phone_fp,(int)strlen(g_trust_phone_fp),pph,sizeof(pph));
    trust_hex_encode((const uint8_t*)g_trust_policy,(int)strlen(g_trust_policy),polh,sizeof(polh));
    trust_hex_encode((const uint8_t*)g_trust_host_pub_pending,(int)strlen(g_trust_host_pub_pending),pendingh,sizeof(pendingh));
    trust_hex_encode((const uint8_t*)g_trust_last_proof,(int)strlen(g_trust_last_proof),lph,sizeof(lph));
    trust_hex_encode((const uint8_t*)g_authority_request_id,(int)strlen(g_authority_request_id),reqh,sizeof(reqh));
    trust_hex_encode((const uint8_t*)g_authority_action,(int)strlen(g_authority_action),acth,sizeof(acth));
    trust_hex_encode((const uint8_t*)g_authority_resource,(int)strlen(g_authority_resource),resh,sizeof(resh));
    trust_hex_encode((const uint8_t*)g_authority_prompt,(int)strlen(g_authority_prompt),prompth,sizeof(prompth));
    snprintf(wire,sizeof(wire),"ZTRUSTUI/2\nSTATE %d\nSERVICE_ALIVE %d\nPAIRED %d\nPROOF_COUNT %u\nAUTH_PENDING %d\nSTATUS_HEX %s\nHOST_NAME_HEX %s\nHOST_FP_HEX %s\nHOST_ID_HEX %s\nPHONE_FP_HEX %s\nPOLICY_HEX %s\nPENDING_HEX %s\nLAST_PROOF_HEX %s\nAUTH_REQUEST_HEX %s\nAUTH_ACTION_HEX %s\nAUTH_RESOURCE_HEX %s\nAUTH_PROMPT_HEX %s\nEND\n",g_trust_state,g_trust_service_alive?1:0,paired?1:0,g_trust_proof_count,g_authority_pending?1:0,sh,nh,fph,ih,pph,polh,pendingh,lph,reqh,acth,resh,prompth);
    return trust_ui_write_file("trust-ui-state",wire);
}
static long long trust_ui_parse_i64(const char* s){long long v=0;int neg=0;if(!s)return 0;if(*s=='-'){neg=1;++s;}while(*s>='0'&&*s<='9'){v=v*10+(*s-'0');++s;}return neg?-v:v;}
static int trust_ui_decode_field(const char* frame,const char* key,char* out,int cap){char hex[1200];int n=trust_frame_field(frame,key,hex,sizeof(hex));if(n<0){if(cap)out[0]=0;return -1;}int d=trust_hex_decode(hex,(uint8_t*)out,cap-1);if(d<0){out[0]=0;return -2;}out[d]=0;return d;}
static int trust_ui_sync_from_service(void){
    char frame[7600],v[64];int n=trust_ui_read_file("trust-ui-state",frame,sizeof(frame));if(n<=0||(!strstr(frame,"ZTRUSTUI/2")&&!strstr(frame,"ZTRUSTUI/1")))return -1;
    int changed=!g_trust_ui_last_frame[0]||!mini_streq(g_trust_ui_last_frame,frame);
    if(changed){int i=0;while(frame[i]&&i<(int)sizeof(g_trust_ui_last_frame)-1){g_trust_ui_last_frame[i]=frame[i];++i;}g_trust_ui_last_frame[i]=0;}
    if(trust_frame_field(frame,"STATE",v,sizeof(v))>=0)g_trust_state=(int)trust_ui_parse_i64(v);
    if(trust_frame_field(frame,"SERVICE_ALIVE",v,sizeof(v))>=0)g_trust_service_alive=trust_ui_parse_i64(v)?1:0;
    if(trust_frame_field(frame,"PAIRED",v,sizeof(v))>=0)g_trust_paired_hint=trust_ui_parse_i64(v)?1:0;
    if(trust_frame_field(frame,"PROOF_COUNT",v,sizeof(v))>=0)g_trust_proof_count=(unsigned int)trust_ui_parse_i64(v);
    (void)trust_ui_decode_field(frame,"STATUS_HEX",g_trust_status,sizeof(g_trust_status));
    (void)trust_ui_decode_field(frame,"HOST_NAME_HEX",g_trust_host_name,sizeof(g_trust_host_name));
    (void)trust_ui_decode_field(frame,"HOST_FP_HEX",g_trust_host_fp,sizeof(g_trust_host_fp));
    (void)trust_ui_decode_field(frame,"HOST_ID_HEX",g_trust_host_identity,sizeof(g_trust_host_identity));
    (void)trust_ui_decode_field(frame,"PHONE_FP_HEX",g_trust_phone_fp,sizeof(g_trust_phone_fp));
    (void)trust_ui_decode_field(frame,"POLICY_HEX",g_trust_policy,sizeof(g_trust_policy));
    (void)trust_ui_decode_field(frame,"PENDING_HEX",g_trust_host_pub_pending,sizeof(g_trust_host_pub_pending));
    (void)trust_ui_decode_field(frame,"LAST_PROOF_HEX",g_trust_last_proof,sizeof(g_trust_last_proof));
    if(trust_frame_field(frame,"AUTH_PENDING",v,sizeof(v))>=0)g_authority_pending=trust_ui_parse_i64(v)?1:0;
    (void)trust_ui_decode_field(frame,"AUTH_REQUEST_HEX",g_authority_request_id,sizeof(g_authority_request_id));
    (void)trust_ui_decode_field(frame,"AUTH_ACTION_HEX",g_authority_action,sizeof(g_authority_action));
    (void)trust_ui_decode_field(frame,"AUTH_RESOURCE_HEX",g_authority_resource,sizeof(g_authority_resource));
    (void)trust_ui_decode_field(frame,"AUTH_PROMPT_HEX",g_authority_prompt,sizeof(g_authority_prompt));
    return changed?1:0;
}
static int trust_ui_send_command(const char* command,const char* binding){
    if(!command)return -1;char wire[900];if(binding&&*binding)snprintf(wire,sizeof(wire),"ZTRUSTUI/2\nCOMMAND %s\nBINDING %s\nEND\n",command,binding);else snprintf(wire,sizeof(wire),"ZTRUSTUI/2\nCOMMAND %s\nEND\n",command);return trust_ui_write_file("trust-ui-command",wire);
}
static void trust_ui_consume_command(void){
    char frame[1024],path[420],cmd[64],binding[600];int n=trust_ui_read_file("trust-ui-command",frame,sizeof(frame));if(n<=0)return;if(trust_ui_path(path,sizeof(path),"trust-ui-command")==0)(void)unlink(path);
    if(trust_frame_field(frame,"COMMAND",cmd,sizeof(cmd))<0)return;
    if(mini_streq(cmd,"FORGET")){g_trust_forget_requested=1;return;}
    if(mini_streq(cmd,"APPROVE")){binding[0]=0;(void)trust_frame_field(frame,"BINDING",binding,sizeof(binding));if(g_trust_state==1&&g_trust_host_pub_pending[0]&&binding[0]&&mini_streq(binding,g_trust_host_pub_pending))g_trust_approve_requested=1;return;}
    if(mini_streq(cmd,"AUTH_APPROVE")||mini_streq(cmd,"AUTH_DENY")){binding[0]=0;(void)trust_frame_field(frame,"BINDING",binding,sizeof(binding));if(g_authority_pending&&binding[0]&&mini_streq(binding,g_authority_request_id))g_authority_decision=mini_streq(cmd,"AUTH_APPROVE")?1:-1;return;}
}

static void trust_set_status(int state,const char* status){g_trust_state=state;if(status){int i=0;while(status[i]&&i<(int)sizeof(g_trust_status)-1){g_trust_status[i]=status[i];++i;}g_trust_status[i]=0;}trust_ui_publish_state();}

static long long trust_parse_i64(const char* s){long long v=0;int neg=0;if(!s)return 0;if(*s=='-'){neg=1;++s;}while(*s>='0'&&*s<='9'){v=v*10+(*s-'0');++s;}return neg?-v:v;}
static int trust_prefix(const char* a,const char* p){if(!a||!p)return 0;for(int i=0;p[i];++i)if(a[i]!=p[i])return 0;return 1;}
static int trust_action_allowed(const char* a){
    if(!a||!*a)return 0;
    return trust_prefix(a,"owner.")||trust_prefix(a,"credential.")||trust_prefix(a,"authority.")||trust_prefix(a,"ops.");
}
static int trust_recv_session_message(int fd,char* out,int cap){int pos=0;out[0]=0;while(pos<cap-1){ssize_t n=recv(fd,out+pos,(size_t)(cap-1-pos),0);if(n<=0)return -1;pos+=(int)n;out[pos]=0;if(strstr(out,"PONG\n")||strstr(out,"PONG\r\n")||strstr(out,"\nEND\n")||strstr(out,"\r\nEND\r\n"))return pos;}return -2;}
static void trust_clear_authority_request(void){g_authority_pending=0;g_authority_decision=0;g_authority_request_id[0]=0;g_authority_action[0]=0;g_authority_resource[0]=0;g_authority_prompt[0]=0;trust_ui_publish_state();}
static void trust_copy_string(char* out,int cap,const char* in){if(!out||cap<1)return;int i=0;if(in)while(in[i]&&i<cap-1){out[i]=in[i];++i;}out[i]=0;}
static int trust_handle_proof_request(int fd,const char* frame){
    char ah[512],rh[1100],ph[800],nonce[128],issueds[64],expiress[64],requestid[96],mode[32];
    ph[0]=0;requestid[0]=0;mode[0]=0;
    if(trust_frame_field(frame,"ACTION_HEX",ah,sizeof(ah))<0||trust_frame_field(frame,"RESOURCE_HEX",rh,sizeof(rh))<0||trust_frame_field(frame,"NONCE",nonce,sizeof(nonce))<0||trust_frame_field(frame,"ISSUED",issueds,sizeof(issueds))<0||trust_frame_field(frame,"EXPIRES",expiress,sizeof(expiress))<0){trust_send_all(fd,"PROOF_RESULT DENY\nREASON malformed\nEND\n");return -1;}
    (void)trust_frame_field(frame,"PROMPT_HEX",ph,sizeof(ph));(void)trust_frame_field(frame,"REQUEST_ID",requestid,sizeof(requestid));(void)trust_frame_field(frame,"MODE",mode,sizeof(mode));
    uint8_t ab[256],rb[512],pb[384];int an=trust_hex_decode(ah,ab,sizeof(ab)-1),rn=trust_hex_decode(rh,rb,sizeof(rb)-1),pn=ph[0]?trust_hex_decode(ph,pb,sizeof(pb)-1):0;if(an<=0||rn<0||pn<0){trust_send_all(fd,"PROOF_RESULT DENY\nREASON encoding\nEND\n");return -2;}ab[an]=0;rb[rn]=0;if(pn>=0)pb[pn]=0;
    if(!trust_action_allowed((const char*)ab)){trust_send_all(fd,"PROOF_RESULT DENY\nREASON action-not-allowed\nEND\n");return -3;}
    long long issued=trust_parse_i64(issueds),expires=trust_parse_i64(expiress);struct timespec nowts;if(clock_gettime(0,&nowts)!=0){trust_send_all(fd,"PROOF_RESULT DENY\nREASON clock\nEND\n");return -4;}long long now=(long long)nowts.tv_sec;
    if(expires<=issued||expires-issued>120||issued<now-30||issued>now+30){trust_send_all(fd,"PROOF_RESULT DENY\nREASON ttl\nEND\n");return -5;}
    int explicit_mode=mini_streq(mode,"EXPLICIT");
    if(explicit_mode){
        if(!requestid[0]){trust_send_all(fd,"PROOF_RESULT DENY\nREASON missing-request-id\nEND\n");return -6;}
        g_authority_decision=0;g_authority_pending=1;trust_copy_string(g_authority_request_id,sizeof(g_authority_request_id),requestid);trust_copy_string(g_authority_action,sizeof(g_authority_action),(const char*)ab);trust_copy_string(g_authority_resource,sizeof(g_authority_resource),(const char*)rb);trust_copy_string(g_authority_prompt,sizeof(g_authority_prompt),pn>0?(const char*)pb:(const char*)ab);trust_ui_publish_state();
        // Wait for the Activity to write a decision into app-private IPC. The
        // phone may initially be locked; only an explicit tap after unlock can approve.
        int decided=0;for(int i=0;i<500&&g_running;++i){trust_ui_consume_command();if(g_authority_decision<0){decided=-1;break;}if(g_authority_decision>0&&!trust_device_locked()){decided=1;break;}usleep(100000);}
        if(decided<0){trust_clear_authority_request();trust_send_all(fd,"PROOF_RESULT DENY\nREASON user-denied\nEND\n");return -7;}
        if(decided==0){trust_clear_authority_request();trust_send_all(fd,"PROOF_RESULT DENY\nREASON approval-timeout\nEND\n");return -8;}
    } else if(trust_device_locked()){trust_send_all(fd,"PROOF_RESULT DENY\nREASON phone-locked\nEND\n");return -9;}
    char msg[2600];snprintf(msg,sizeof(msg),"ZTRUST/2|OWNER_PROOF|%s|%s|%s|%s|%s|%s|%s",g_trust_host_fp,g_trust_phone_fp,ah,rh,nonce,issueds,expiress);
    uint8_t sig[192];int slen=trust_sign((const uint8_t*)msg,(int)strlen(msg),sig,sizeof(sig));if(slen<=0){if(explicit_mode)trust_clear_authority_request();trust_send_all(fd,"PROOF_RESULT DENY\nREASON signing\nEND\n");return -10;}char sighex[512];trust_hex_encode(sig,slen,sighex,sizeof(sighex));char wire[800];snprintf(wire,sizeof(wire),"PROOF_RESULT OK\nSIGNATURE %s\nEND\n",sighex);if(trust_send_all(fd,wire)!=0){if(explicit_mode)trust_clear_authority_request();return -11;}
    ++g_trust_proof_count;snprintf(g_trust_last_proof,sizeof(g_trust_last_proof),"%s -> %.72s",(const char*)ab,(const char*)rb);if(explicit_mode)trust_clear_authority_request();else trust_ui_publish_state();return 0;
}

static int trust_auth_session(void){
    int fd=socket(AF_INET,SOCK_STREAM,0);if(fd<0)return -1;struct sockaddr_in sa;memset(&sa,0,sizeof(sa));sa.sin_family=AF_INET;sa.sin_port=trust_net_u16((uint16_t)ZORIN_TRUST_PORT);sa.sin_addr.s_addr=0x0100007fU;if(connect(fd,&sa,(unsigned int)sizeof(sa))!=0){close(fd);return -2;}
    char frame[4096],hostpub[512],hostnonce[128],hostname[128],hostidentity[96];if(trust_recv_frame(fd,frame,sizeof(frame))<0){close(fd);return -3;}if(!strstr(frame,"ZTRUST/2")){close(fd);return -4;}if(trust_frame_field(frame,"HOST_PUB",hostpub,sizeof(hostpub))<0||trust_frame_field(frame,"HOST_NONCE",hostnonce,sizeof(hostnonce))<0){close(fd);return -5;}if(trust_frame_field(frame,"HOST_NAME",hostname,sizeof(hostname))<0)snprintf(hostname,sizeof(hostname),"UNKNOWN HOST");if(trust_frame_field(frame,"HOST_IDENTITY",hostidentity,sizeof(hostidentity))<0)snprintf(hostidentity,sizeof(hostidentity),"UNKNOWN");
    uint8_t hostder[256];int hostderlen=trust_hex_decode(hostpub,hostder,sizeof(hostder));char fp[128]="?";if(hostderlen<=0||trust_fingerprint_der(hostder,hostderlen,fp,sizeof(fp))<0){close(fd);return -6;}snprintf(g_trust_host_name,sizeof(g_trust_host_name),"%s",hostname);snprintf(g_trust_host_fp,sizeof(g_trust_host_fp),"%s",fp);snprintf(g_trust_host_identity,sizeof(g_trust_host_identity),"%s",hostidentity);snprintf(g_trust_host_pub_current,sizeof(g_trust_host_pub_current),"%s",hostpub);
    int same=trust_host_is_trusted(hostpub);if(!same){snprintf(g_trust_host_pub_pending,sizeof(g_trust_host_pub_pending),"%s",hostpub);trust_set_status(1,"UNKNOWN HOST - APPROVAL REQUIRED");close(fd);return 1001;}
    trust_set_status(2,"MUTUAL AUTHENTICATION");uint8_t phder[256];int phlen=trust_get_public_der(phder,sizeof(phder));if(phlen<=0){trust_set_status(-1,"ANDROID KEYSTORE ERROR");close(fd);return -7;}char phonepub[512];trust_hex_encode(phder,phlen,phonepub,sizeof(phonepub));trust_fingerprint_der(phder,phlen,g_trust_phone_fp,sizeof(g_trust_phone_fp));uint8_t nonce[32];if(trust_random(nonce,sizeof(nonce))<0){close(fd);return -8;}char phnonce[80];trust_hex_encode(nonce,sizeof(nonce),phnonce,sizeof(phnonce));
    char proof[1536];snprintf(proof,sizeof(proof),"ZTRUST/2|PHONE|%s|%s|%s|%s",hostnonce,phnonce,hostpub,phonepub);uint8_t sig[160];int slen=trust_sign((const uint8_t*)proof,(int)strlen(proof),sig,sizeof(sig));if(slen<=0){trust_set_status(-1,"PHONE SIGNATURE FAILED");close(fd);return -9;}char sighex[384];trust_hex_encode(sig,slen,sighex,sizeof(sighex));int initial_locked=trust_device_locked();char wire[2300];snprintf(wire,sizeof(wire),"PHONE_PUB %s\nPHONE_NONCE %s\nPHONE_SIG %s\nPHONE_STATE %s\nEND\n",phonepub,phnonce,sighex,initial_locked?"LOCKED":"UNLOCKED");if(trust_send_all(fd,wire)!=0){close(fd);return -10;}
    if(trust_recv_frame(fd,frame,sizeof(frame))<0){close(fd);return -11;}if(strstr(frame,"AUTH PAIR_REQUIRED")){trust_set_status(1,"HOST REQUIRES --PAIR-ONCE");close(fd);return 1003;}if(!strstr(frame,"AUTH OK")){trust_set_status(-1,"HOST REJECTED DEVICE");close(fd);return -12;}char hostsighex[384],policy[96];if(trust_frame_field(frame,"HOST_SIG",hostsighex,sizeof(hostsighex))<0){close(fd);return -13;}if(trust_frame_field(frame,"POLICY",policy,sizeof(policy))<0)snprintf(policy,sizeof(policy),"owner-workstation");uint8_t hostsig[192];int hslen=trust_hex_decode(hostsighex,hostsig,sizeof(hostsig));if(hslen<=0){close(fd);return -14;}char hproof[1536];snprintf(hproof,sizeof(hproof),"ZTRUST/2|HOST|%s|%s|%s|%s",phnonce,hostnonce,phonepub,hostpub);if(!trust_verify_host(hostder,hostderlen,(const uint8_t*)hproof,(int)strlen(hproof),hostsig,hslen)){trust_set_status(-1,"HOST SIGNATURE INVALID");close(fd);return -15;}
    snprintf(g_trust_policy,sizeof(g_trust_policy),"%s",policy);trust_set_status(3,"DEVICE TRUST ESTABLISHED");
    while(g_running&&!g_trust_forget_requested){
        int locked=trust_device_locked();
        if(trust_send_all(fd,locked?"POLL LOCKED\n":"POLL UNLOCKED\n")!=0)break;
        char smsg[4096];int sn=trust_recv_session_message(fd,smsg,sizeof(smsg));if(sn<0)break;
        if(strstr(smsg,"PROOF_REQUEST")){
            trust_handle_proof_request(fd,smsg);
            char pong[128];int pn=trust_recv_session_message(fd,pong,sizeof(pong));if(pn<0||!strstr(pong,"PONG"))break;
        } else if(!strstr(smsg,"PONG")) break;
        usleep(1000000);
    }
    close(fd);if(g_trust_state==3)trust_set_status(0,"USB DEVICE TRUST LOST");return 0;
}

static void* trust_worker(void* arg){
    (void)arg;uint8_t phder[256];int phlen=trust_get_public_der(phder,sizeof(phder));if(phlen>0)trust_fingerprint_der(phder,phlen,g_trust_phone_fp,sizeof(g_trust_phone_fp));else snprintf(g_trust_phone_fp,sizeof(g_trust_phone_fp),"KEYSTORE ERROR");trust_ui_publish_state();
    while(g_running){
        trust_ui_consume_command();
        if(g_trust_forget_requested){if(g_trust_host_pub_current[0])(void)trust_host_remove(g_trust_host_pub_current);g_trust_forget_requested=0;g_trust_approve_requested=0;g_trust_host_pub_pending[0]=0;g_trust_host_pub_current[0]=0;snprintf(g_trust_host_name,sizeof(g_trust_host_name),"<NONE>");snprintf(g_trust_host_fp,sizeof(g_trust_host_fp),"<NONE>");snprintf(g_trust_policy,sizeof(g_trust_policy),"none");trust_set_status(0,"WORKSTATION REVOKED");}
        if(g_trust_state==1&&g_trust_approve_requested&&g_trust_host_pub_pending[0]){(void)trust_host_add(g_trust_host_pub_pending);snprintf(g_trust_host_pub_current,sizeof(g_trust_host_pub_current),"%s",g_trust_host_pub_pending);g_trust_approve_requested=0;trust_set_status(0,"WORKSTATION APPROVED - AUTHENTICATING");}
        int rc=trust_auth_session();(void)rc;for(int i=0;i<20&&g_running;++i){trust_ui_consume_command();if(g_trust_approve_requested||g_trust_forget_requested)break;usleep(100000);}
    }g_trust_thread_started=0;return 0;
}

static void trust_start_worker(void){if(g_trust_thread_started)return;g_trust_thread_started=1;pthread_t t;if(pthread_create(&t,0,trust_worker,0)==0)pthread_detach(t);else{g_trust_thread_started=0;trust_set_status(-1,"TRUST THREAD FAILED");}}
