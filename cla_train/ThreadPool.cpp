#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum):stop_(false)
{
    for (size_t ii = 0; ii < threadnum; ii++)
    {
        threads_.emplace_back([this]
        {
            printf("create thread(%ld).\n", syscall(SYS_gettid));
            //std::cout << "子线程：" << std::this_thread::get_id() << std::endl;

            while(stop_ == false)           
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    this->condition_.wait(lock, [this]
                    {
                        return ((this->stop_== true) ||(this->taskqueue_.empty() == false));
                    });

                    if ((this->stop_ == true) && (this->taskqueue_.empty() == true)) return;

                    task = std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }

                printf("thread is %ld.\n", syscall(SYS_gettid));
                task();
            }
        });
    }
}

void ThreadPool::addtask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }

    condition_.notify_one();
}

ThreadPool::~ThreadPool()
{
    stop_ = true;

    condition_.notify_all();

    for (std::thread &th : threads_)
        th.join();
}

/*void show(int no, const std::string &name)
{
    printf("我是第%d号报文%s。.\n", no, name.c_str());
}

void test()
{
    printf("我有一只小小鸟.\n");
}

int main()
{
    ThreadPool threadpool(3);

    std::string name = "西施";
    threadpool.addtask(std::bind(show, 8, name));
    sleep(1);

    threadpool.addtask(std::bind(test));
    sleep(1);

    threadpool.addtask(std::bind([]{ printf("我是一只傻傻鸟.\n");}));
    sleep(1);
}*/
