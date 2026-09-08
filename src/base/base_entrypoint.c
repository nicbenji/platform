internal int entrypoint(int argc, char **argv);

internal int main_thread_entrypoint(int argc, char **argv) {
    return entrypoint(argc, argv);
}
