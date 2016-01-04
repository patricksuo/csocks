SOURCES = [
    "buff.c",
    "hashmap.c",
    "list.c",
    "malloc.c",
    "malloc_diagnosis.c",
    "task.c",
    "utils.c"
]

# LIBS = []
# LIBPATH = ["/usr/lib", "/usr/local/lib", ]

env = Environment(CC='gcc', CCFLAGS='-c -Wall -g -rdynamic')


hashmap_test = Program("build/hashmap_test", SOURCES + ["hashmap_test.c"])
task_test = Program("build/task_test", SOURCES + ["task_test.c"])
malloc_diagnosis_test = Program("build/malloc_diagnosis_test", SOURCES + ["malloc_diagnosis_test.c"])


env.Alias('dev',[hashmap_test, task_test, malloc_diagnosis_test])


def print_config(msg, two_dee_iterable):
    # this function is handy and can be used for other configuration-printing tasks
    print
    print msg
    print
    for key, val in two_dee_iterable:
        print "    %-20s %s" % (key, str(val))
    print

def config_h_build(target, source, env):
    config_h_defines = dict(
        debug_alloc = 0,
    )
    for key in config_h_defines.keys():
        if ARGUMENTS.get(key): config_h_defines[key] = 1

    print_config("Generating config.h with the following settings:",
        config_h_defines.items())
    for a_target, a_source in zip(target, source):
        config_h = file(str(a_target), "w")
        config_h_in = file(str(a_source), "r")
        config_h.write(config_h_in.read() % config_h_defines)
        config_h_in.close()
        config_h.close()

env.AlwaysBuild(env.Command('config.h', 'config.h.in', config_h_build))
