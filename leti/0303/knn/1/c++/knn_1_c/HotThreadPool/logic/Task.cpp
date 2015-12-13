#include "Task.h"

uint32_t Task::all_objects_count = 0;

Task::Task(uint32_t _time_to_perform) : uid(all_objects_count++), elapsed_time(0), result(0), time_to_perform(_time_to_perform) {
}

// ������� �����, ��������� � ������ ������ ������
uint32_t Task::get_elapsed_time() const {
	return elapsed_time;
}

// ������� ������� ��������� ������ ������
uint32_t Task::get_result() const {
	return result;
}

// ������� �����, ������� ��������� ��� ������� ������ ������
uint32_t Task::get_time_to_perform() const {
	return time_to_perform;
}

// ������� ���������� ������������� ������
uint32_t Task::get_uid() const {
	return uid;
}

// ��������� ������
uint32_t Task::run_task() {
	try {
		// ������� ���� ���� ������� ���������
		while (elapsed_time < time_to_perform) {
			// ����� ����� ������ �������, ��������� ������� �������� ������
			// �������� ��������� ����� 999 �����������
			boost::this_thread::sleep(boost::posix_time::millisec(1000));
			// ����������� ������� ��������� ������
			elapsed_time++;
			// ��������� ������� �������
			result++;
		}
	}
	catch (boost::thread_interrupted) {
		// ������� ���������� � ������ ���������� ���������� �������
		throw boost::thread_interrupted();
	}
	return result;
}

Task::~Task() {
}
