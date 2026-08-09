typedef struct AIBinder AIBinder;
AIBinder* AServiceManager_checkService(const char* instance){(void)instance;return (AIBinder*)0;}
int AIBinder_ping(AIBinder* binder){(void)binder;return -1;}
_Bool AIBinder_isRemote(const AIBinder* binder){(void)binder;return 0;}
_Bool AIBinder_isAlive(const AIBinder* binder){(void)binder;return 0;}
void AIBinder_decStrong(AIBinder* binder){(void)binder;}
