#ifndef TOOL_H
#define TOOL_H

#include <atomic>
#include <mutex>

class Tool {
public:
	Tool();

	inline bool isTaken() { return taken; };

	inline int takenBy() { return pizzaioloNo; };

	void take(int pizzaioloNo);

	void leave();
private:
	std::atomic<bool> taken;
	int pizzaioloNo = -1;
};

#endif /* TOOL_H */