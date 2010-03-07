#include <stdlib.h>
#include <pthread.h>
#include "ruby.h"

pthread_t gRubyThread;
pthread_mutex_t gMainLock;
pthread_cond_t gRubyDone;

// aRubyProgram:: program node for the interpreter to run
void* the_ruby_thread(void* aRubyProgram)
{
    printf("Ruby thread is starting interpreter\n");
    ruby_run_node(aRubyProgram);

    printf("Ruby thread is done, waking up C program...\n");
    pthread_mutex_lock(&gMainLock);
    pthread_cond_signal(&gRubyDone);
    pthread_mutex_unlock(&gMainLock);

    pthread_exit(NULL);
}

// RUBY_GLOBAL_SETUP

void the_c_program()
{
    char* file = "hello.rb"; // the file to run
    int fake_argc = 2;
    char* fake_args[fake_argc];
    fake_args[0] = "ruby";
    fake_args[1] = file;
    char** fake_argv = fake_args;

    printf("C program is calling ruby_sysinit()\n");
    ruby_sysinit(&fake_argc, &fake_argv);

    printf("C program is calling RUBY_INIT_STACK()\n");
    RUBY_INIT_STACK;

    printf("C program is calling ruby_init()\n");
    ruby_init();
    rb_evalstring("puts \"Boy\"");

    printf("C program is calling ruby_init_loadpath()\n");
    ruby_init_loadpath();

    printf("C program is loading file: %s\n", file);
    void* rubyProgram = ruby_options(fake_argc, fake_argv);

    pthread_mutex_init(&gMainLock, NULL);
    pthread_cond_init(&gRubyDone, NULL);
    pthread_create(&gRubyThread, NULL,
                   the_ruby_thread, rubyProgram);

    printf("C program is putting Ruby thread in control...\n");
    pthread_mutex_lock(&gMainLock);
    pthread_cond_wait(&gRubyDone, &gMainLock); // blocking call
    pthread_mutex_unlock(&gMainLock);

    printf("C program is back in control, exiting...\n");
    pthread_mutex_destroy(&gMainLock);
    pthread_cond_destroy(&gRubyDone);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    the_c_program();
    return 0;
}
