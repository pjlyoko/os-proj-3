#include "Supplier.h"


Supplier::Supplier(int numb, mutex *mutexFridge, int *fridge, mutex *mutexWriter) : numb(numb),
																					mutexFridge(mutexFridge),
																					fridge(fridge),
																					mutexWriter(mutexWriter) {
	threadS = new thread(&Supplier::threadSupplier, this);
}

Supplier::~Supplier() {
	threadS->join();
	delete threadS;
}

void Supplier::threadSupplier() {
	float deliverTime = 15;
	chrono::_V2::steady_clock::time_point begin, dur;

	int fridgeSize = 10;

	while(!end) {

		mutexWriter->lock();
		mvprintw(numb + 30, 0, "Dostawca %d: Czeka na dostęp do lodowki", numb);
		mutexWriter->unlock();

		while(!mutexFridge->try_lock() && !end);

		mutexWriter->lock();
		mvprintw(numb + 30, 0, "Dostawca %d: uzupelnia lodowke              ", numb);
		mutexWriter->unlock();

		for(int i = 0; i < fridgeSize; i++) {
			fridge[i] = 10;
		}
		usleep(breaks);

		mutexWriter->lock();
		mvprintw(5, 50, "Lodowka");
		for(int i = 0; i < fridgeSize; i++) {
			mvprintw(6, 50 + 3 * i, "%d ", fridge[i]);
		}
		mutexWriter->unlock();

		mutexFridge->unlock();

		begin = chrono::steady_clock::now();
		dur = chrono::steady_clock::now();

		while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < deliverTime && !end) {
			mutexWriter->lock();
			mvprintw(numb + 30, 0, "Dostawca %d: Wyjechal po skladniki %d %%              ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / deliverTime * 100));
			mutexWriter->unlock();

			usleep(breaks);
			dur = chrono::steady_clock::now();
		}
	}
}

void Supplier::threadClose() {
	end = true;
}
