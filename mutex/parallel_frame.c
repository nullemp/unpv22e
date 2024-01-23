struct work {
    void (*run) (void *arg);
    void *arg;
}

void Tworker()  {
    while(1) {
        struct work *work;
        wait_until(has_new_work || all_done) {
            work = get_work();
        }
        if (!work)
            break;
        else {
            work->run(work->arg);
            release(work);
        }
    }
}