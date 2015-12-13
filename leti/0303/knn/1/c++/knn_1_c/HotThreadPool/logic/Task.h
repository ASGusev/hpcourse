#ifndef _TASK_H_
#define _TASK_H_

#pragma once

#include <stdint.h>

#include <boost\thread\thread.hpp>

class Task {
	// ������� ���������� ��������� �� ����� ������ ��������
	static uint32_t all_objects_count;
	// �����, ������� ������ � ������� ������� ������ � ������
	uint32_t elapsed_time;
	// ��������� ������
	uint32_t result;
	// �����, ������� ��������� ��� ������� ������
	uint32_t time_to_perform;
	// ���������� ������������� ������� ������
	uint32_t uid;

public:
	explicit Task(uint32_t _time_to_perform = 0);

	// ������� �����, ��������� � ������ ������ ������
	uint32_t get_elapsed_time() const;
	// ������� ������� ��������� ������ ������
	uint32_t get_result() const;
	// ������� �����, ������� ��������� ��� ������� ������ ������
	uint32_t get_time_to_perform() const;
	// ������� ���������� ������������� ������
	uint32_t get_uid() const;
	// ��������� ������
	uint32_t run_task();

	virtual ~Task();
};

#endif