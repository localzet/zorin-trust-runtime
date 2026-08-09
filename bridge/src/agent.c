// Zorin ADB Shell Core v3.0
// Authenticated, read-only localhost capability backend for Zorin Trust Runtime.
// No arbitrary command execution. Every exposed operation is a fixed diagnostic.
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long size_t;
typedef long ssize_t;

struct utsname
{
    char sysname[65], nodename[65], release[65], version[65], machine[65], domainname[65];
};
struct in_addr
{
    uint32_t s_addr;
};
struct sockaddr_in
{
    uint16_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    uint8_t sin_zero[8];
};
typedef struct AIBinder AIBinder;

extern int snprintf(char *, size_t, const char *, ...);
extern int open(const char *, int, ...);
extern ssize_t read(int, void *, size_t);
extern ssize_t write(int, const void *, size_t);
extern int close(int);
extern int access(const char *, int);
extern int getpid(void);
extern unsigned int getuid(void);
extern unsigned int getgid(void);
extern int getpagesize(void);
extern int uname(struct utsname *);
extern size_t strlen(const char *);
extern void *memset(void *, int, size_t);
extern void *memcpy(void *, const void *, size_t);
extern char *strstr(const char *, const char *);
extern ssize_t readlink(const char *, char *, size_t);
extern int socket(int, int, int);
extern int bind(int, const void *, unsigned int);
extern int listen(int, int);
extern int accept4(int, void *, void *, int);
extern int setsockopt(int, int, int, const void *, unsigned int);
extern int fork(void);
extern int waitpid(int, int *, int);
extern int pipe2(int[2], int);
extern void _exit(int);
extern int *__errno(void);
extern long syscall(long, ...);
extern int __system_property_get(const char *, char *);
extern AIBinder *AServiceManager_checkService(const char *);
extern int AIBinder_ping(AIBinder *);
extern void AIBinder_decStrong(AIBinder *);

#define O_RDONLY 0
#define O_RDWR 2
#define O_CLOEXEC 0x80000
#define O_NONBLOCK 0x800
#define F_OK 0
#define AF_INET 2
#define SOCK_STREAM 1
#define SOL_SOCKET 1
#define SO_REUSEADDR 2
#define DEFAULT_PORT 47471
#define PROP_VALUE_MAX 92
#define SIGSYS 31
#define MFD_CLOEXEC 1

#if defined(__aarch64__)
#define GETTID_NR 178
#define BPF_NR 280
#define PERF_EVENT_OPEN_NR 241
#define USERFAULTFD_NR 282
#define IO_URING_SETUP_NR 425
#define PIDFD_OPEN_NR 434
#define MEMFD_CREATE_NR 279
#define MEMBARRIER_NR 283
#define LANDLOCK_CREATE_RULESET_NR 444
#define SETNS_NR 268
#elif defined(__x86_64__)
#define GETTID_NR 186
#define BPF_NR 321
#define PERF_EVENT_OPEN_NR 298
#define USERFAULTFD_NR 323
#define IO_URING_SETUP_NR 425
#define PIDFD_OPEN_NR 434
#define MEMFD_CREATE_NR 319
#define MEMBARRIER_NR 324
#define LANDLOCK_CREATE_RULESET_NR 444
#define SETNS_NR 308
#elif defined(__arm__)
#define GETTID_NR 224
#define BPF_NR 386
#define PERF_EVENT_OPEN_NR 364
#define USERFAULTFD_NR 388
#define IO_URING_SETUP_NR 425
#define PIDFD_OPEN_NR 434
#define MEMFD_CREATE_NR 385
#define MEMBARRIER_NR 389
#define LANDLOCK_CREATE_RULESET_NR 444
#define SETNS_NR 375
#else
#define GETTID_NR 224
#define BPF_NR 357
#define PERF_EVENT_OPEN_NR 336
#define USERFAULTFD_NR 374
#define IO_URING_SETUP_NR 425
#define PIDFD_OPEN_NR 434
#define MEMFD_CREATE_NR 356
#define MEMBARRIER_NR 375
#define LANDLOCK_CREATE_RULESET_NR 444
#define SETNS_NR 346
#endif

static char g_token[96];
static int g_require_auth = 1;

static int last_errno(void)
{
    int *p = __errno();
    return p ? *p : -1;
}
static uint16_t net_u16(uint16_t v) { return (uint16_t)((v << 8) | (v >> 8)); }

static int streq(const char *a, const char *b)
{
    while (*a && *b && *a == *b)
    {
        ++a;
        ++b;
    }
    return *a == 0 && *b == 0;
}
static int streq_prefix(const char *a, const char *b)
{
    while (*b)
    {
        if (*a++ != *b++)
            return 0;
    }
    return 1;
}
static void trim(char *s)
{
    int n = (int)strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = 0;
}
static void copy_str(char *out, int cap, const char *s)
{
    if (!out || cap < 1)
        return;
    int i = 0;
    if (s)
        while (s[i] && i < cap - 1)
        {
            out[i] = s[i];
            ++i;
        }
    out[i] = 0;
}
static int token_equal(const char *a, const char *b)
{
    // Constant-work comparison for the fixed maximum token length. This is local-only,
    // but avoiding an early-out costs nothing and keeps the boundary clean.
    unsigned int diff = 0;
    int enda = 0, endb = 0;
    for (int i = 0; i < 95; ++i)
    {
        unsigned char ca = (!enda && a && a[i]) ? (unsigned char)a[i] : 0;
        unsigned char cb = (!endb && b && b[i]) ? (unsigned char)b[i] : 0;
        diff |= (unsigned int)(ca ^ cb);
        if (!ca)
            enda = 1;
        if (!cb)
            endb = 1;
    }
    return diff == 0;
}
static int parse_port(const char *s)
{
    int v = 0;
    if (!s || !*s)
        return DEFAULT_PORT;
    while (*s >= '0' && *s <= '9')
    {
        v = v * 10 + (*s - '0');
        ++s;
        if (v > 65535)
            return DEFAULT_PORT;
    }
    return v > 0 ? v : DEFAULT_PORT;
}
static int read_small(const char *path, char *out, int cap)
{
    if (!out || cap < 2)
        return -1;
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        out[0] = 0;
        return -last_errno();
    }
    ssize_t n = read(fd, out, (size_t)cap - 1);
    int e = last_errno();
    close(fd);
    if (n < 0)
    {
        out[0] = 0;
        return -e;
    }
    out[n] = 0;
    trim(out);
    return (int)n;
}
static int count_file_lines(const char *path)
{
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return -last_errno();
    char b[2048];
    int lines = 0;
    ssize_t n;
    while ((n = read(fd, b, sizeof(b))) > 0)
        for (ssize_t i = 0; i < n; ++i)
            if (b[i] == '\n')
                ++lines;
    int e = last_errno();
    close(fd);
    return n < 0 ? -e : lines;
}
static int status_value(const char *key, char *out, int cap)
{
    char b[8192];
    if (read_small("/proc/self/status", b, sizeof(b)) < 0)
        return -1;
    char *p = strstr(b, key);
    if (!p)
        return -1;
    p += strlen(key);
    while (*p == ' ' || *p == '\t')
        ++p;
    int i = 0;
    while (*p && *p != '\n' && i < cap - 1)
        out[i++] = *p++;
    out[i] = 0;
    trim(out);
    return i;
}
static void get_context(char *out, int cap)
{
    int rc = read_small("/proc/self/attr/current", out, cap);
    if (rc < 0)
        snprintf(out, (size_t)cap, "ERRNO %d", -rc);
}
static int prop_get(const char *key, char *out, int cap)
{
    char b[PROP_VALUE_MAX];
    int n = __system_property_get(key, b);
    if (n <= 0)
    {
        if (cap)
            out[0] = 0;
        return 0;
    }
    copy_str(out, cap, b);
    return n;
}
static void send_text(int fd, const char *s)
{
    if (s)
        write(fd, s, strlen(s));
}
static int readlink_small(const char *p, char *out, int cap)
{
    ssize_t n = readlink(p, out, (size_t)cap - 1);
    if (n < 0)
    {
        out[0] = 0;
        return -last_errno();
    }
    out[n] = 0;
    return (int)n;
}

static void append_kv(char *out, int cap, int *pos, const char *k, const char *v)
{
    if (*pos >= cap - 2)
        return;
    *pos += snprintf(out + *pos, (size_t)(cap - *pos), "%s=%s\n", k, v ? v : "");
}
static void append_i(char *out, int cap, int *pos, const char *k, long v)
{
    if (*pos >= cap - 2)
        return;
    *pos += snprintf(out + *pos, (size_t)(cap - *pos), "%s=%ld\n", k, v);
}

static void command_hello(int fd)
{
    // HELLO intentionally exposes no privileged values; it only advertises protocol/auth mode.
    send_text(fd, g_require_auth ? "ZORIN_ADB_CORE/3 auth=required\n" : "ZORIN_ADB_CORE/3 auth=disabled\n");
}

static void command_status(int fd)
{
    char seccomp[32] = "?", capeff[64] = "?", nnp[32] = "?", ctx[192], enforce[32] = "?", out[2048];
    int pos = 0;
    status_value("Seccomp:", seccomp, sizeof(seccomp));
    status_value("CapEff:", capeff, sizeof(capeff));
    status_value("NoNewPrivs:", nnp, sizeof(nnp));
    get_context(ctx, sizeof(ctx));
    int b = access("/dev/binder", F_OK) == 0, hb = access("/dev/hwbinder", F_OK) == 0, vb = access("/dev/vndbinder", F_OK) == 0;
    int net = access("/proc/net/tcp", F_OK) == 0;
    int vfd = open("/dev/vndbinder", O_RDWR | O_CLOEXEC);
    int vopen = vfd >= 0;
    if (vfd >= 0)
        close(vfd);
    int erc = read_small("/sys/fs/selinux/enforce", enforce, sizeof(enforce));
    if (erc < 0)
        snprintf(enforce, sizeof(enforce), "ERRNO %d", -erc);
    char t[192];
    snprintf(t, sizeof(t), "%s", getuid() == 0 ? "root" : "adb-shell");
    append_kv(out, sizeof(out), &pos, "mode", t);
    append_i(out, sizeof(out), &pos, "uid", getuid());
    append_i(out, sizeof(out), &pos, "gid", getgid());
    append_i(out, sizeof(out), &pos, "pid", getpid());
    append_kv(out, sizeof(out), &pos, "context", ctx);
    append_kv(out, sizeof(out), &pos, "seccomp", seccomp);
    append_kv(out, sizeof(out), &pos, "no_new_privs", nnp);
    append_kv(out, sizeof(out), &pos, "capeff", capeff);
    snprintf(t, sizeof(t), "%d/%d/%d", b, hb, vb);
    append_kv(out, sizeof(out), &pos, "binder_nodes", t);
    append_i(out, sizeof(out), &pos, "vndbinder_open", vopen);
    append_i(out, sizeof(out), &pos, "proc_net", net);
    append_kv(out, sizeof(out), &pos, "selinux_enforce", enforce);
    send_text(fd, out);
}

static void command_services(int fd)
{
    const char *names[] = {
        "activity", "package", "window", "power", "display", "SurfaceFlinger", "sensorservice", "connectivity", "netd", "vold",
        "media.audio_flinger", "permission", "mount", "keystore2", "stats", "incident", "gatekeeper", "gpu", "input", "batterystats",
        "deviceidle", "thermalservice", "storaged", "usagestats", "jobscheduler", "appops", "role", "shortcut", "notification", "location",
        "wifi", "bluetooth_manager", "media.extractor", "media.metrics", "drm.drmManager", "android.security.authorization.IKeystoreAuthorization/default"};
    char out[4096];
    int pos = 0, found = 0, ping_fail = 0, total = (int)(sizeof(names) / sizeof(names[0]));
    pos += snprintf(out + pos, sizeof(out) - pos, "tested=%d\nfound=", total);
    for (int i = 0; i < total; ++i)
    {
        AIBinder *s = AServiceManager_checkService(names[i]);
        if (!s)
            continue;
        int pr = AIBinder_ping(s);
        AIBinder_decStrong(s);
        if (pr != 0)
        {
            ++ping_fail;
            continue;
        }
        if (found++)
            pos += snprintf(out + pos, sizeof(out) - pos, ",");
        pos += snprintf(out + pos, sizeof(out) - pos, "%s", names[i]);
        if (pos > (int)sizeof(out) - 256)
            break;
    }
    pos += snprintf(out + pos, sizeof(out) - pos, "\ncount=%d\nping_fail=%d\n", found, ping_fail);
    send_text(fd, out);
}

static void command_kernel(int fd)
{
    char out[4096], b[1536], v[512];
    int pos = 0;
    struct utsname u;
    if (uname(&u) == 0)
    {
        snprintf(v, sizeof(v), "%s %s", u.sysname, u.release);
        append_kv(out, sizeof(out), &pos, "kernel", v);
        append_kv(out, sizeof(out), &pos, "machine", u.machine);
    }
    else
        append_kv(out, sizeof(out), &pos, "kernel", "unavailable");
    append_i(out, sizeof(out), &pos, "page_size", getpagesize());
    if (read_small("/proc/version", (char *)b, sizeof(b)) >= 0)
        append_kv(out, sizeof(out), &pos, "proc_version", (char *)b);
    if (read_small("/proc/uptime", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "uptime", v);
    if (read_small("/proc/loadavg", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "loadavg", v);
    if (read_small("/proc/sys/kernel/osrelease", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "osrelease", v);
    if (read_small("/proc/sys/kernel/hostname", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "hostname", v);
    send_text(fd, out);
}

static void parse_interfaces(const char *in, char *out, int cap)
{
    int pos = 0;
    const char *p = in;
    int line = 0;
    while (*p)
    {
        const char *e = p;
        while (*e && *e != '\n')
            ++e;
        ++line;
        if (line > 2)
        {
            const char *c = p;
            while (c < e && *c != ':')
                ++c;
            if (c < e)
            {
                const char *s = p;
                while (s < c && (*s == ' ' || *s == '\t'))
                    ++s;
                const char *z = c;
                while (z > s && (z[-1] == ' ' || z[-1] == '\t'))
                    --z;
                if (z > s)
                {
                    if (pos && pos < cap - 1)
                        out[pos++] = ',';
                    while (s < z && pos < cap - 1)
                        out[pos++] = *s++;
                }
            }
        }
        p = *e ? e + 1 : e;
    }
    out[pos] = 0;
}
static void command_network(int fd)
{
    char out[4096], b[4096], ifs[512], route[1024];
    int pos = 0;
    int rc = read_small("/proc/net/dev", b, sizeof(b));
    if (rc >= 0)
    {
        parse_interfaces(b, ifs, sizeof(ifs));
        append_kv(out, sizeof(out), &pos, "interfaces", ifs);
    }
    else
    {
        snprintf(ifs, sizeof(ifs), "ERRNO %d", -rc);
        append_kv(out, sizeof(out), &pos, "interfaces", ifs);
    }
    int n = count_file_lines("/proc/net/route");
    append_i(out, sizeof(out), &pos, "route_lines", n > 0 ? n - 1 : n);
    n = count_file_lines("/proc/net/tcp");
    append_i(out, sizeof(out), &pos, "tcp_entries", n > 0 ? n - 1 : n);
    n = count_file_lines("/proc/net/tcp6");
    append_i(out, sizeof(out), &pos, "tcp6_entries", n > 0 ? n - 1 : n);
    n = count_file_lines("/proc/net/udp");
    append_i(out, sizeof(out), &pos, "udp_entries", n > 0 ? n - 1 : n);
    n = count_file_lines("/proc/net/udp6");
    append_i(out, sizeof(out), &pos, "udp6_entries", n > 0 ? n - 1 : n);
    rc = read_small("/proc/net/route", route, sizeof(route));
    if (rc >= 0)
    {
        char *nl = strstr(route, "\n");
        if (nl)
        {
            char *p = nl + 1;
            char *e = strstr(p, "\n");
            if (e)
                *e = 0;
            trim(p);
            append_kv(out, sizeof(out), &pos, "route_first", p);
        }
    }
    send_text(fd, out);
}

static void read_sysctl_kv(char *out, int cap, int *pos, const char *key, const char *path)
{
    char v[128];
    int rc = read_small(path, v, sizeof(v));
    if (rc >= 0)
        append_kv(out, cap, pos, key, v);
    else
    {
        snprintf(v, sizeof(v), "ERRNO %d", -rc);
        append_kv(out, cap, pos, key, v);
    }
}
static void command_security(int fd)
{
    char out[4096], v[512], ctx[192], sec[32] = "?", caps[64] = "?", nnp[32] = "?";
    int pos = 0;
    get_context(ctx, sizeof(ctx));
    append_kv(out, sizeof(out), &pos, "context", ctx);
    status_value("Seccomp:", sec, sizeof(sec));
    append_kv(out, sizeof(out), &pos, "seccomp", sec);
    status_value("NoNewPrivs:", nnp, sizeof(nnp));
    append_kv(out, sizeof(out), &pos, "no_new_privs", nnp);
    status_value("CapEff:", caps, sizeof(caps));
    append_kv(out, sizeof(out), &pos, "capeff", caps);
    int rc = read_small("/sys/fs/selinux/enforce", v, sizeof(v));
    if (rc >= 0)
        append_kv(out, sizeof(out), &pos, "selinux_enforce", v);
    else
    {
        snprintf(v, sizeof(v), "ERRNO %d", -rc);
        append_kv(out, sizeof(out), &pos, "selinux_enforce", v);
    }
    const char *props[] = {"ro.boot.verifiedbootstate", "ro.boot.vbmeta.device_state", "ro.boot.flash.locked", "ro.debuggable", "ro.secure"};
    const char *keys[] = {"verified_boot", "vbmeta_state", "flash_locked", "debuggable", "ro_secure"};
    for (int i = 0; i < 5; ++i)
    {
        if (prop_get(props[i], v, sizeof(v)))
            append_kv(out, sizeof(out), &pos, keys[i], v);
        else
            append_kv(out, sizeof(out), &pos, keys[i], "not_exposed");
    }
    read_sysctl_kv(out, sizeof(out), &pos, "kptr_restrict", "/proc/sys/kernel/kptr_restrict");
    read_sysctl_kv(out, sizeof(out), &pos, "dmesg_restrict", "/proc/sys/kernel/dmesg_restrict");
    read_sysctl_kv(out, sizeof(out), &pos, "perf_event_paranoid", "/proc/sys/kernel/perf_event_paranoid");
    read_sysctl_kv(out, sizeof(out), &pos, "unprivileged_bpf_disabled", "/proc/sys/kernel/unprivileged_bpf_disabled");
    read_sysctl_kv(out, sizeof(out), &pos, "unprivileged_userfaultfd", "/proc/sys/vm/unprivileged_userfaultfd");
    read_sysctl_kv(out, sizeof(out), &pos, "ptrace_scope", "/proc/sys/kernel/yama/ptrace_scope");
    send_text(fd, out);
}

static void command_filesystem(int fd)
{
    char out[3072], v[1024];
    int pos = 0;
    int n = count_file_lines("/proc/self/mountinfo");
    append_i(out, sizeof(out), &pos, "mountinfo_lines", n);
    if (read_small("/proc/self/cgroup", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "cgroup", v);
    if (readlink_small("/proc/self/ns/mnt", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "ns_mnt", v);
    if (readlink_small("/proc/self/ns/pid", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "ns_pid", v);
    if (readlink_small("/proc/self/ns/net", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "ns_net", v);
    if (readlink_small("/proc/self/ns/user", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "ns_user", v);
    if (read_small("/proc/filesystems", v, sizeof(v)) >= 0)
    {
        int lines = 1;
        for (char *p = v; *p; ++p)
            if (*p == '\n')
                ++lines;
        append_i(out, sizeof(out), &pos, "filesystem_types_visible", lines);
    }
    send_text(fd, out);
}

static void command_process(int fd)
{
    char out[3072], v[512];
    int pos = 0;
    append_i(out, sizeof(out), &pos, "pid", getpid());
    append_i(out, sizeof(out), &pos, "uid", getuid());
    append_i(out, sizeof(out), &pos, "gid", getgid());
    append_i(out, sizeof(out), &pos, "tid", syscall(GETTID_NR));
    const char *fields[] = {"Threads:", "VmRSS:", "VmSize:", "TracerPid:", "Seccomp:", "NoNewPrivs:", "CapEff:"};
    const char *keys[] = {"threads", "rss", "vm_size", "tracer_pid", "seccomp", "no_new_privs", "cap_eff"};
    for (int i = 0; i < 7; ++i)
        if (status_value(fields[i], v, sizeof(v)) >= 0)
            append_kv(out, sizeof(out), &pos, keys[i], v);
    if (read_small("/proc/loadavg", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "loadavg", v);
    send_text(fd, out);
}

static void command_report(int fd)
{
    char ctx[192], sec[32] = "?", caps[64] = "?", enforce[32] = "?", v[128], out[3072];
    int pos = 0;
    struct utsname u;
    get_context(ctx, sizeof(ctx));
    status_value("Seccomp:", sec, sizeof(sec));
    status_value("CapEff:", caps, sizeof(caps));
    int erc = read_small("/sys/fs/selinux/enforce", enforce, sizeof(enforce));
    if (erc < 0)
        snprintf(enforce, sizeof(enforce), "ERRNO %d", -erc);
    pos += snprintf(out + pos, sizeof(out) - pos, "ZORIN ADB CORE REPORT v3\n");
    if (uname(&u) == 0)
    {
        snprintf(v, sizeof(v), "%s %s / %s", u.sysname, u.release, u.machine);
        append_kv(out, sizeof(out), &pos, "kernel", v);
    }
    append_i(out, sizeof(out), &pos, "uid", getuid());
    append_i(out, sizeof(out), &pos, "gid", getgid());
    append_i(out, sizeof(out), &pos, "pid", getpid());
    append_kv(out, sizeof(out), &pos, "context", ctx);
    append_kv(out, sizeof(out), &pos, "seccomp", sec);
    append_kv(out, sizeof(out), &pos, "capeff", caps);
    append_kv(out, sizeof(out), &pos, "selinux_enforce", enforce);
    append_kv(out, sizeof(out), &pos, "proc_net_tcp", access("/proc/net/tcp", F_OK) == 0 ? "VISIBLE" : "DENIED");
    int vfd = open("/dev/vndbinder", O_RDWR | O_CLOEXEC);
    append_kv(out, sizeof(out), &pos, "vndbinder_open", vfd >= 0 ? "YES" : "NO");
    if (vfd >= 0)
        close(vfd);
    send_text(fd, out);
}

struct probe_result
{
    long rc;
    int err;
};

static void probe_format(char *out, int cap, int sig, const struct probe_result *r, int got)
{
    if (sig)
    {
        snprintf(out, (size_t)cap, sig == SIGSYS ? "SIGSYS/SECCOMP" : "SIGNAL %d", sig);
        return;
    }
    if (!got)
    {
        copy_str(out, cap, "NO_RESULT");
        return;
    }
    if (r->rc >= 0)
        snprintf(out, (size_t)cap, "OK rc=%ld", r->rc);
    else
        snprintf(out, (size_t)cap, "ERRNO %d", r->err);
}

static void run_probe_kind(int kind, char *out, int cap)
{
    int pfd[2] = {-1, -1};
    if (pipe2(pfd, O_CLOEXEC) != 0)
    {
        snprintf(out, (size_t)cap, "PIPE ERRNO %d", last_errno());
        return;
    }
    int pid = fork();
    if (pid == 0)
    {
        close(pfd[0]);
        struct probe_result r;
        r.rc = -1;
        r.err = 0;
        if (kind == 1)
        {
            unsigned char params[256];
            memset(params, 0, sizeof(params));
            r.rc = syscall(IO_URING_SETUP_NR, 2, params);
        }
        else if (kind == 2)
        {
            unsigned char attr[128];
            memset(attr, 0, sizeof(attr));
            ((uint32_t *)attr)[0] = 2;
            ((uint32_t *)attr)[1] = 4;
            ((uint32_t *)attr)[2] = 4;
            ((uint32_t *)attr)[3] = 1;
            r.rc = syscall(BPF_NR, 0, attr, 64);
        }
        else if (kind == 3)
        {
            unsigned char attr[128];
            memset(attr, 0, sizeof(attr));
            ((uint32_t *)attr)[0] = 1;
            ((uint32_t *)attr)[1] = 128;
            ((uint64_t *)(attr + 8))[0] = 0;
            ((uint64_t *)(attr + 40))[0] = 1;
            r.rc = syscall(PERF_EVENT_OPEN_NR, attr, 0, -1, -1, 0);
        }
        else if (kind == 4)
        {
            r.rc = syscall(USERFAULTFD_NR, O_CLOEXEC | O_NONBLOCK);
        }
        else if (kind == 5)
        {
            r.rc = syscall(PIDFD_OPEN_NR, getpid(), 0);
        }
        else if (kind == 6)
        {
            static const char name[] = "zorin-shell-probe";
            r.rc = syscall(MEMFD_CREATE_NR, name, MFD_CLOEXEC);
        }
        else if (kind == 7)
        {
            r.rc = syscall(MEMBARRIER_NR, 0, 0, 0);
        }
        else if (kind == 8)
        {
            r.rc = syscall(LANDLOCK_CREATE_RULESET_NR, 0, 0, 1);
        }
        else if (kind == 9)
        {
            int nfd = open("/proc/self/ns/net", O_RDONLY | O_CLOEXEC);
            if (nfd < 0)
            {
                r.rc = -1;
                r.err = last_errno();
            }
            else
            {
                r.rc = syscall(SETNS_NR, nfd, 0);
                if (r.rc < 0)
                    r.err = last_errno();
                close(nfd);
            }
        }
        if (r.rc < 0 && r.err == 0)
            r.err = last_errno();
        if ((kind == 1 || kind == 2 || kind == 3 || kind == 4 || kind == 5 || kind == 6) && r.rc >= 0)
            close((int)r.rc);
        write(pfd[1], &r, sizeof(r));
        close(pfd[1]);
        _exit(0);
    }
    close(pfd[1]);
    if (pid < 0)
    {
        snprintf(out, (size_t)cap, "FORK ERRNO %d", last_errno());
        close(pfd[0]);
        return;
    }
    struct probe_result r;
    memset(&r, 0, sizeof(r));
    ssize_t n = read(pfd[0], &r, sizeof(r));
    close(pfd[0]);
    int st = 0;
    if (waitpid(pid, &st, 0) < 0)
    {
        snprintf(out, (size_t)cap, "WAIT ERRNO %d", last_errno());
        return;
    }
    int sig = st & 0x7f;
    probe_format(out, cap, sig, &r, n == (ssize_t)sizeof(r));
}

static void command_probes(int fd)
{
    const char *keys[] = {"io_uring_setup", "bpf_map_create", "perf_event_open", "userfaultfd", "pidfd_open", "memfd_create", "membarrier_query", "landlock_abi", "setns_self_net"};
    char out[3072], v[128];
    int pos = 0;
    for (int i = 0; i < 9; ++i)
    {
        run_probe_kind(i + 1, v, sizeof(v));
        append_kv(out, sizeof(out), &pos, keys[i], v);
    }
    send_text(fd, out);
}

static void path_visibility(char *out, int cap, int *pos, const char *key, const char *path)
{
    char v[256];
    int rc = read_small(path, v, sizeof(v));
    if (rc >= 0)
        append_kv(out, cap, pos, key, "VISIBLE");
    else
    {
        snprintf(v, sizeof(v), "ERRNO %d", -rc);
        append_kv(out, cap, pos, key, v);
    }
}

static void command_procfs(int fd)
{
    char out[4096], v[512];
    int pos = 0;
    path_visibility(out, sizeof(out), &pos, "pid1_status", "/proc/1/status");
    path_visibility(out, sizeof(out), &pos, "pid1_cmdline", "/proc/1/cmdline");
    path_visibility(out, sizeof(out), &pos, "modules", "/proc/modules");
    path_visibility(out, sizeof(out), &pos, "interrupts", "/proc/interrupts");
    path_visibility(out, sizeof(out), &pos, "cpuinfo", "/proc/cpuinfo");
    path_visibility(out, sizeof(out), &pos, "meminfo", "/proc/meminfo");
    path_visibility(out, sizeof(out), &pos, "vmstat", "/proc/vmstat");
    path_visibility(out, sizeof(out), &pos, "pressure_cpu", "/proc/pressure/cpu");
    int n = count_file_lines("/proc/modules");
    append_i(out, sizeof(out), &pos, "module_lines", n);
    n = count_file_lines("/proc/interrupts");
    append_i(out, sizeof(out), &pos, "interrupt_lines", n);
    if (readlink_small("/proc/1/ns/mnt", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "pid1_ns_mnt", v);
    else
        append_kv(out, sizeof(out), &pos, "pid1_ns_mnt", "DENIED");
    if (readlink_small("/proc/1/ns/pid", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "pid1_ns_pid", v);
    else
        append_kv(out, sizeof(out), &pos, "pid1_ns_pid", "DENIED");
    if (readlink_small("/proc/1/ns/net", v, sizeof(v)) >= 0)
        append_kv(out, sizeof(out), &pos, "pid1_ns_net", v);
    else
        append_kv(out, sizeof(out), &pos, "pid1_ns_net", "DENIED");
    send_text(fd, out);
}

static const char *authorize(char *cmd)
{
    trim(cmd);
    if (streq(cmd, "HELLO"))
        return cmd;
    if (!g_require_auth)
        return cmd;
    if (!streq_prefix(cmd, "AUTH "))
        return 0;
    char *token = cmd + 5;
    char *p = token;
    while (*p && *p != ' ')
        ++p;
    if (!*p)
        return 0;
    *p++ = 0;
    while (*p == ' ')
        ++p;
    if (!token_equal(token, g_token))
        return 0;
    return p;
}

static void handle_client(int fd)
{
    char buf[512];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0)
        return;
    buf[n] = 0;
    char *cmd = (char *)authorize(buf);
    if (!cmd)
    {
        send_text(fd, "ERR auth-required-or-failed\n");
        return;
    }
    if (streq(cmd, "HELLO"))
        command_hello(fd);
    else if (streq(cmd, "STATUS"))
        command_status(fd);
    else if (streq(cmd, "SERVICES"))
        command_services(fd);
    else if (streq(cmd, "KERNEL"))
        command_kernel(fd);
    else if (streq(cmd, "NETWORK"))
        command_network(fd);
    else if (streq(cmd, "SECURITY"))
        command_security(fd);
    else if (streq(cmd, "FILESYSTEM"))
        command_filesystem(fd);
    else if (streq(cmd, "PROCESS"))
        command_process(fd);
    else if (streq(cmd, "REPORT"))
        command_report(fd);
    else if (streq(cmd, "PROBES"))
        command_probes(fd);
    else if (streq(cmd, "PROCFS"))
        command_procfs(fd);
    else
        send_text(fd, "ERR unknown-command; allowed=HELLO,STATUS,SERVICES,KERNEL,NETWORK,SECURITY,FILESYSTEM,PROCESS,REPORT,PROBES,PROCFS\n");
}

int main(int argc, char **argv)
{
    int port = DEFAULT_PORT, allow_root = 0;
    g_token[0] = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (streq(argv[i], "--allow-root"))
            allow_root = 1;
        else if (streq(argv[i], "--insecure-localhost"))
            g_require_auth = 0;
        else if (streq(argv[i], "--port") && i + 1 < argc)
            port = parse_port(argv[++i]);
        else if (streq(argv[i], "--token") && i + 1 < argc)
        {
            char *src = argv[++i];
            copy_str(g_token, sizeof(g_token), src);
            // Reduce token lifetime in /proc/<pid>/cmdline after startup.
            for (size_t j = 0; j < strlen(src); ++j)
                src[j] = 'x';
        }
    }
    if (getuid() == 0 && !allow_root)
    {
        send_text(2, "Zorin ADB Core refuses uid=0 by default. Pass --allow-root only for deliberate local testing.\n");
        return 13;
    }
    if (g_require_auth && strlen(g_token) < 16)
    {
        send_text(2, "Zorin ADB Core v3 requires --token <random-token> (>=16 chars), or explicit --insecure-localhost for debugging.\n");
        return 14;
    }
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        return 10;
    int one = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = net_u16((uint16_t)port);
    sa.sin_addr.s_addr = 0x0100007fU;
    if (bind(s, &sa, sizeof(sa)) != 0)
        return 11;
    if (listen(s, 4) != 0)
        return 12;
    char banner[256];
    snprintf(banner, sizeof(banner), "Zorin ADB Shell Core v3 listening on 127.0.0.1:%d uid=%u pid=%d auth=%s\n", port, getuid(), getpid(), g_require_auth ? "required" : "disabled");
    write(1, banner, strlen(banner));
    for (;;)
    {
        int c = accept4(s, 0, 0, O_CLOEXEC);
        if (c < 0)
            continue;
        handle_client(c);
        close(c);
    }
    return 0;
}
