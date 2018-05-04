#pragma once
/*class EventSequence
{
public:
	std::vector<EventTranslator> events;
	EventSequence ();
	void update(void* self) { 
		for (auto& ref : events) {
			ref.perform(self);
		}
	};
	~EventSequence();
};

class EventTranslator {
public:
	enum EventType { 
		relocate,
		move,
		rotate,
		target,
		scale,
		destroy,
		instantize,
		set,
		cppline,
		log,
		ifelse,
		forwhile
	} commandType;
	
	std::vector<void*> params;
	void perform(void* self) {
		if (commandType == log) {
			printf((char*)params[0]);
			printf("\n");
		}
	};
};*/