#include "Worker.h"
#include "ThreadPool.h"

Worker::Worker(ThreadPool *_thread_pool,
			   boost::mutex *_io_mutex,
			   boost::condition_variable *_condition_v,
			   uint32_t _thread_timeout,
			   bool _worker_is_hot,
			   uint32_t _thread_uid
			  )
	: thread_pool(_thread_pool),
	  worker_task(nullptr),
	  io_mutex(_io_mutex),
	  condition_v(_condition_v),
	  is_stoped(false),
	  worker_thread(nullptr),
	  worker_is_free(true),
	  worker_is_hot(_worker_is_hot),
	  thread_uid(_thread_uid),
	  thread_timeout(_thread_timeout)  
{
}

Task* Worker::get_task() const
{
	return (worker_task) ? worker_task : nullptr;
}

int32_t Worker::get_task_result() const
{
	return (worker_task) ? worker_task->get_result() : -1;
}

int32_t Worker::get_task_uid() const
{
	return (worker_task) ? worker_task->get_uid() : -1;
}

uint32_t Worker::get_thread_uid() const
{
	return thread_uid;
}

boost::thread* Worker::get_worker_thread() const
{
	return (worker_thread) ? worker_thread : nullptr;
}

void Worker::operator()()
{
	// ���� worker �� ���������� ��� ����� ������������ ����� ������
	while (!is_stoped) {
		boost::mutex::scoped_lock scop_lock(task_mutex);
		try {
			// ���� ������� �������� ������� �������
			if (worker_is_hot) {
				// ���� worker �� �������� �� ��� ����� �� �����
				if (!worker_task) {
					// ���� ������ �� �������� ������ worker�
					while (!is_stoped) {
						// ������ ������ �� ���� ��� ���, ���� �� ��� ���-�� ������
						Task *stolen_task = thread_pool->get_task_from_queue();
						(stolen_task) ? set_worker_task(stolen_task) : condition_v->wait(scop_lock);
					}
				}
			}
			// � ��� �������� ����� �� �����
			else {
				// ���� woker� �� �������� ����� ��� ������
				if (!worker_task) {
					// ���� ������ �� �������� ������ worker�
					while (!is_stoped) {
						// ������ ���� ������ 
						Task *stolen_task = thread_pool->get_task_from_queue();
						if (stolen_task) {
							set_worker_task(stolen_task);
						}
						// ������ �� ������� ��������� � �������� ����� ����� ���������
						else {
							// ��������� ����� �� �����������
							boost::system_time const tim�_to_remove = boost::get_system_time() + boost::posix_time::seconds(thread_timeout);
							// ���� ����� ��� ����
							if (condition_v->timed_wait(scop_lock, tim�_to_remove)) {
								// ������ ������ 
								stolen_task = thread_pool->get_task_from_queue();
								// ���� ������ ���������
								if (stolen_task)
									set_worker_task(stolen_task);
							}
							// ������� ������ ���
							else {
								// ������������� ������ ������
								stop_worker_task();
								// ������� ������� ����� �� ����
								thread_pool->remove_thread_from_pool(this);
								return;
							}
						}
					}
				}
			}
		}
		// ���������� ���������� �������
		catch (boost::thread_interrupted) {
			if (worker_task) {
				// ����� ����� �����/������
				boost::mutex::scoped_lock scop_lock(*io_mutex);
				// ������� ���� ���� ���������
				std::cout << "Task[" << worker_task->get_uid() << "] with labor time: " 
					      << worker_task->get_time_to_perform() << " seconds interrupted in Thread["
						  << thread_uid << "]" << std::endl << ">> Task result: \"" << worker_task->get_result() 
						  << "\" and time elapsed: " << worker_task->get_elapsed_time() << " seconds."
						  << std::endl;
			}
			// ���� worker ��� �������� ��� ��������, �� ������������� ������
			if (!is_stoped) stop_working_thread();
		}
	}
}

void Worker::set_worker_task(Task *_worker_task)
{
	// � ��� ��������� ����� �������! ��������� ���� �� ����!
	worker_task = _worker_task;
	{
		boost::mutex::scoped_lock scop_lock(*io_mutex);
		std::cout << std::endl << std::endl << "Task[" << worker_task->get_uid() << "] with labor time: " 
				  << worker_task->get_time_to_perform() << " seconds. >> Starting in Thread["
				  << thread_uid << "]" << std::endl;
	}

	worker_is_free = false;
	// � ���� �� ���� ����� ������
	thread_pool->inc_active_thread();
	// ��������� ������� � ��� ��������� � ������
	worker_task->run_task();

	{
		// ��� ������ ������� �������, ��������� ���� �� ����!
		boost::mutex::scoped_lock scop_lock(*io_mutex);
		std::cout << "Task[" << worker_task->get_uid() << "] with labor time:" 
				  << worker_task->get_time_to_perform() << "seconds. >> Finished in Thread["
				  << thread_uid << "] >> Task result: \"" << worker_task->get_result() << "\"" << std::endl;
	}

	stop_working_thread();
}

void Worker::set_working_thread(boost::thread *_worker_thread) {
	worker_thread = _worker_thread;
}

void Worker::stop_worker_task() {
	if (!is_stoped) {
		is_stoped = true;
		{
			boost::mutex::scoped_lock scop_lock(*io_mutex);
			std::cout << "Thread[" << thread_uid << "] was stoped" << std::endl;
		}
	}
}

void Worker::stop_working_thread() {
	thread_pool->dec_active_thread();
	if (worker_task) {
		delete worker_task;
		worker_task = nullptr;
	}
	worker_is_free = true;
}

bool Worker::worker_is_stoped() const
{
	return is_stoped;
}

bool Worker::worker_ready_for_task() const
{
	return (worker_is_free && !is_stoped) ? true : false;
}

Worker::~Worker() {
	if (worker_task != nullptr) {
		delete worker_task;
		worker_task = nullptr;
	}
}
