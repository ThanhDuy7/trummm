#include "main.h"

class imp_res : public Restaurant
{
	public:
		class queue;
		class stack;
	private:
		customer* X; //a specified customer is on table
		queue* list_table; // customers are waiting
		queue* list_waiting;
		queue* list_waiting_order;
		bool used_BLUE;
		int count_main; // the number of seat

	public:	
		imp_res() {
			X = nullptr;
			used_BLUE = 0;
			count_main = 0;
			list_table = new queue();
			list_waiting = new queue();
			list_waiting_order = new queue();
		}
		~imp_res() {
			if (list_table->count == 1) {
				delete X;
				X = nullptr;
				count_main = 0;
			}
			else if (list_table->count > 1) {
				customer* tmp = X->next;
				while (tmp != X) {
					customer* bin = tmp;
					tmp = tmp->next;
					delete bin;
					bin = nullptr;
				}
				delete X;
				X = nullptr;
				count_main = 0;
			}
			delete list_table;
			delete list_waiting;
			delete list_waiting_order;
		}

		void RED(string name, int energy)
		{
			//cout << name << " " << energy << endl;
			// only customer'energy != 0 and the number of seat is enough or the number of waiting seat's enough
			if (energy != 0 && (list_table->count < MAXSIZE || list_waiting->count < MAXSIZE)) {
				if (list_table->count == 0) {
					customer* cus = new customer(name, energy, nullptr, nullptr);
					X = cus;
					list_table->enqueue(cus);
					++count_main;
				}
				// create a Dlinkedlist like a table when put head linked to tail and tail linked to head 
				else if (list_table->count == 1) {
					if (X->name != name) {
						customer* cus = new customer(name, energy, nullptr, nullptr);
						list_table->enqueue(cus);
						X->next = cus;
						X->prev = cus;
						cus->next = X;
						cus->prev = X;
						X = cus;
						++count_main;
					}
				}
				else if(list_table->count > 1) {
					//___________check if name exist in the restaurant
					bool exist_name = 0; // this vari is to check if exist name in restaurant
					if (this->used_BLUE) {
						exist_name = 0;
						this->used_BLUE = 0;
					}
					else if (list_table->contain_name(name) || list_waiting->contain_name(name)) {
						exist_name = 1;
					}
					//handle if number of customer smaller than MAXSIZE/2
					if (list_table->count < MAXSIZE / 2 && !exist_name ) {
						if (energy >= X->energy) {
							customer* cus = new customer(name, energy, nullptr, nullptr);
							list_table->enqueue(cus);
							cus->next = X->next;
							X->next->prev = cus;
							X->next = cus;
							cus->prev = X;
							X = cus;
							++count_main;
						}
						else {
							customer* cus = new customer(name, energy, nullptr, nullptr);
							list_table->enqueue(cus);
							cus->prev = X->prev;
							X->prev->next = cus;
							X->prev = cus;
							cus->next = X;
							X = cus;
							++count_main;
						}
					}
					// handle if number of customer bigger than OR equal MAXSIZE/2  .
					else if (list_table->count >= (MAXSIZE / 2) && list_table->count < MAXSIZE && !exist_name) {
						customer* tmp = X;
						customer* temp = tmp;
						customer* cus = new customer(name, energy, nullptr, nullptr);
						list_table->enqueue(cus);
						int RES = abs(energy - tmp->energy);
						tmp = tmp->next;
						while (tmp != X) {
							if (abs(energy - tmp->energy) > RES) {
								RES = abs(energy - tmp->energy);
								temp = tmp;
							}
							tmp = tmp->next;
						}
						X = temp;
						if (energy >= X->energy) {
							cus->next = X->next;
							X->next->prev = cus;
							X->next = cus;
							cus->prev = X;
							X = cus;
							++count_main;
						}
						else {
							cus->prev = X->prev;
							X->prev->next = cus;
							X->prev = cus;
							cus->next = X;
							X = cus;
							++count_main;
						}
					}
					//handle when the number of customer bigger than MAXSIZE, we put them into waiting seat
					else if (list_table->count >= MAXSIZE && list_waiting->count < MAXSIZE && !exist_name) {
						if (list_waiting->count == 0) {
							customer* cus = new customer(name, energy, nullptr, nullptr);
							list_waiting->enqueue(cus);
							list_waiting_order->enqueue(cus);
						}
						else {
							//________________________________________________
							if (!exist_name) {
								customer* cus = new customer(name, energy, nullptr, nullptr);
								list_waiting->enqueue(cus);
								list_waiting_order->enqueue(cus);
								//++count_waiting;
							}
						}
					}
				}
			}
		}
		void BLUE(int num)
		{
			//cout << "blue "<< num << endl;
			if (num > 0) {
				if (list_table->count == 1) {
					list_table->dequeue();
					delete X;
					X = nullptr;
					count_main = 0;
				}
				else if (list_table->count > 1) {
					if (num < list_table->count) {
						for (int i = 0; i < num; i++) {
							customer* tmp = X;
							while (true) {
								if (tmp->name == list_table->front()->name) {
									break;
								}
								tmp = tmp->next;
							}
							if (tmp->energy > 0) X = tmp->next;
							else if (tmp->energy < 0) X = tmp->prev;
							tmp->prev->next = tmp->next;
							tmp->next->prev = tmp->prev;
							delete tmp;
							list_table->dequeue();
							count_main--;
						}
						if (list_table->count == 1) {
							X->next = nullptr;
							X->prev = nullptr;
						}
					}
					else if(num >= list_table->count) {
						list_table->clear();
						customer* tmp = X->next;
						while (tmp != X) {
							customer* bin = tmp;
							tmp = tmp->next;
							delete bin;
							bin = nullptr;
						}
						delete this->X;
						X = nullptr;
						count_main = 0;
					}
				}
				//sau khi duoi khach ta xep cac vi khach dang ngoi cho vao trong ban
				if (list_waiting->count > 0 && (MAXSIZE - list_table->count >= list_waiting->count)) {
					int dem = list_waiting->count;
					for (int i = 0; i < dem; ++i) {
						this->used_BLUE = 1;
						RED(list_waiting->front()->name, list_waiting->front()->energy);
						list_waiting_order->remove_cus(list_waiting->front());
						list_waiting->dequeue();
					}
				}
				else if (list_waiting->count > 0 && (MAXSIZE - list_table->count) < list_waiting->count && (MAXSIZE - list_table->count > 0)) {
					int dem = MAXSIZE - list_table->count;
					for (int i = 0; i < dem; ++i) {
						this->used_BLUE = 1;
						RED(list_waiting->front()->name, list_waiting->front()->energy);
						list_waiting_order->remove_cus(list_waiting->front());
						list_waiting->dequeue();
					}
				}
			}
		}

		void swap(int i, int j) {
			string name = list_waiting->get(i)->name;
			int energy = list_waiting->get(i)->energy;
			list_waiting->get(i)->name = list_waiting->get(j)->name;
			list_waiting->get(i)->energy = list_waiting->get(j)->energy;
			list_waiting->get(j)->name = name;
			list_waiting->get(j)->energy = energy;
		}
		void sort_segment(int step,int n, int incr, int& N) {
			for (int i = step + incr; i < n; i += incr) {
				for (int j = i; (j >= incr) && abs(list_waiting->get(j)->energy) >= abs(list_waiting->get(j - incr)->energy); j -= incr) {
					if (abs(list_waiting->get(j)->energy) == abs(list_waiting->get(j - incr)->energy)) {
						if (list_waiting_order->index_name(list_waiting->get(j)->name) > list_waiting_order->index_name(list_waiting->get(j - incr)->name)) {
							break;
						}
					}
					swap(j, j - incr);
					++N;
				}
			}
		}
		void shellsort(int index) {
			int n = index + 1;
			int N = 0;
			for (int i = n / 2; i >= 1; i /= 2) { // For each increment
				for (int j = 0; j < i; j++) { // Sort each sublist
					sort_segment(j, n , i, N);
				}
			}
			int x = N % MAXSIZE;
			BLUE(x);
		}
		void PURPLE()
		{
			//cout << "PURPLE" << endl;
			if (list_waiting->count > 1) {
					customer* tmp = list_waiting->head;
					int index = 0;
					int max = abs(tmp->energy);
					for (int i = 0; i < list_waiting->count; i++) {
						if (abs(tmp->energy) >= max) {
							index = i;
							max = abs(tmp->energy);
						}
						tmp = tmp->next;
					}
					shellsort(index);
				
			}
		}

		void REVERSAL()
		{
			//cout << "reversal" << endl;
			if (list_table->count > 2) {
				if (X->energy > 0) {
					stack* list_duong = new stack();
					stack* list_am = new stack();
					customer* tmp = X;
					int cycle = 0;
					while (cycle < 2) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						if (tmp->energy > 0) {
							list_duong->push(tmp);
						}
						else if (tmp->energy < 0) {
							list_am->push(tmp);
						}
						tmp = tmp->prev;
					}
					string nameofX = X->name;
					tmp = X;
					cycle = 0;
					while (cycle < 2) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						if (tmp->energy > 0) {
							tmp->name = list_duong->top()->name;
							tmp->energy = list_duong->top()->energy;
							list_duong->pop();
						}
						else if (tmp->energy < 0) {
							tmp->name = list_am->top()->name;
							tmp->energy = list_am->top()->energy;
							list_am->pop();
						}
						tmp = tmp->prev;
					}
					tmp = X;
					cycle = 0;
					while (cycle < 2) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						if (tmp->name == nameofX) {
							X = tmp;
							break;
						}
						tmp = tmp->next;
					}
					delete list_duong;
					delete list_am;
					list_duong = nullptr;
					list_am = nullptr;
				}
				else if (X->energy < 0) {
					stack* list_am = new stack();
					stack* list_duong = new stack();
					customer* tmp = X;
					int cycle = 0;
					while (cycle < 2) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						if (tmp->energy < 0) {
							list_am->push(tmp);
						}
						else if (tmp->energy > 0) {
							list_duong->push(tmp);
						}
						tmp = tmp->prev;
					}
					string nameofX = X->name;
					tmp = X;
					cycle = 0;
					while (cycle < 2) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						if (tmp->energy < 0) {
							tmp->name = list_am->top()->name;
							tmp->energy = list_am->top()->energy;
							list_am->pop();
						}
						else if (tmp->energy > 0) {
							tmp->name = list_duong->top()->name;
							tmp->energy = list_duong->top()->energy;
							list_duong->pop();
						}
						tmp = tmp->prev;
					}
					tmp = X;
					cycle = 0;
					while (cycle < 2) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						if (tmp->name == nameofX) {
							X = tmp;
							break;
						}
						tmp = tmp->next;
					}
					delete list_am;
					delete list_duong;
					list_am = nullptr;
					list_duong = nullptr;
				}
			}
		}

		void UNLIMITED_VOID()
		{
			//cout << "unlimited_void" << endl;
			if (list_table->count >= 4) {
				// tmp luc nay dang tro toi next cua phan tu thu 4 
				if (list_table->count == 4) {
					customer* tmp = X;
					customer* smallest = X;
					for (int i = 0; i < 4; i++) {
						if (tmp->energy <= smallest->energy) {
							smallest = tmp;
						}
						tmp = tmp->next;
					}
					for (int i = 0; i < 4; i++) {
						smallest->print();
						smallest = smallest->next;
					}
				}
				if (list_table->count > 4) {
					queue* currentmin = new queue();
					queue* tmpmin = new queue();
					customer* tmp = X;
					customer* run = tmp->next;
					int min = list_table->sumEnergy();
					int cycle = 0;
					while (true) {
						if (tmp == X) ++cycle;
						if (cycle >= 2) break;
						tmpmin->enqueue(tmp);
						run = tmp->next;
						for (int i = 0; i < 3; i++) {
							tmpmin->enqueue(run);
							run = run->next;
						}
						if (tmpmin->sumEnergy() <= min ) {
							if (tmpmin->sumEnergy() == min) {
								if (tmpmin->count >= currentmin->count) {
									currentmin->copy(tmpmin);
									min = tmpmin->sumEnergy();
								}
							}
							else {
								currentmin->copy(tmpmin);
								min = tmpmin->sumEnergy();
							}
						}
						while (run != tmp) {
							tmpmin->enqueue(run);
							if (tmpmin->sumEnergy() <= min) {
								if (tmpmin->sumEnergy() == min) {
									if (tmpmin->count >= currentmin->count) {
										currentmin->copy(tmpmin);
										min = tmpmin->sumEnergy();
									}
								}
								else {
									currentmin->copy(tmpmin);
									min = tmpmin->sumEnergy();
								}
							}
							run = run->next;
						}
						tmp = tmp->next;
						tmpmin->clear();
					}
					customer* emp = currentmin->head;
					int minnode = currentmin->head->energy;
					string name = currentmin->head->name;
					while (emp != nullptr) {
						if (emp->energy < minnode) {
							name = emp->name;
							minnode = emp->energy;
						}
						emp = emp->next;
					}
					while (name != currentmin->head->name) {
						customer* tmp = currentmin->head;
						currentmin->dequeue();
						currentmin->enqueue(tmp);
					}
					currentmin->print_waiting();
					delete currentmin;
					delete tmpmin;
				}
			}
		}

		void DOMAIN_EXPANSION()
		{
			//cout << "domain_expansion " << line + 1 << endl;
			if (list_table->count + list_waiting->count > 1) {
				int total_duong = 0;
				int total_chulinh = 0;
				int soluongduong = 0;
				int soluongam = 0;
				customer* tmp = list_table->head;
				while (tmp != nullptr) {
					if (tmp->energy > 0) { 
						total_duong += tmp->energy;
						++soluongduong;
					}
					else if (tmp->energy < 0) { 
						total_chulinh += tmp->energy;
						++soluongam;
					}
					tmp = tmp->next;
				}
				if (list_waiting->count > 0) {
					if (list_waiting->count == 1) {
						if (list_waiting->head->energy > 0) { 
							total_duong += list_waiting->head->energy;
						}
						else if (list_waiting->head->energy < 0) { 
							total_chulinh += list_waiting->head->energy;
						}
					}
					else {
						tmp = list_waiting->head;
						while (tmp != nullptr) {
							if (tmp->energy > 0) { 
								total_duong += tmp->energy;
							}
							else if (tmp->energy < 0) { 
								total_chulinh += tmp->energy;
							}
							tmp = tmp->next;
						}
					}
				}
				total_chulinh = abs(total_chulinh);
				//tien hanh duoi khach la chu linh energy < 0
					if (total_duong >= total_chulinh) {
						customer* tmp = list_waiting_order->tail;
						// in ten oan linh bi duoi
						while (tmp != nullptr) {
							if (tmp->energy < 0) {
								tmp->print();
							}
							tmp = tmp->prev;
						}
						//tien hanh duoi oan linh
						tmp = list_waiting_order->head;
						while (tmp != nullptr) {
							if (tmp->energy < 0) {
								customer* bin = tmp;
								tmp = tmp->next;
								list_waiting->remove_cus(bin);
								list_waiting_order->remove_cus(bin);
								continue;
							}
							tmp = tmp->next;
						}
						//-------------------
						tmp = list_table->tail;
						if (soluongam > 0) {
							while (tmp != nullptr) {
								if (tmp->energy < 0) {
									break;
								}
								tmp = tmp->prev;
							}
							while (true) {
								if (X->name == tmp->name) {
									break;
								}
								X = X->next;
							}	
						}
						//___________
						tmp = list_table->tail;
						while (tmp != nullptr) {
							if (tmp->energy < 0) {
								customer* bin = tmp;
								bin->print();
								tmp = tmp->prev;
								list_table->remove_cus(bin);
								continue;
							}
							tmp = tmp->prev;
						}
						//_____________
						if (soluongam > 0) {
							customer* temp = X->next;
							while (temp != X) {
								if (temp->energy < 0) {
									customer* bin = temp;
									temp = temp->next;
									bin->prev->next = bin->next;
									bin->next->prev = bin->prev;
									delete bin;
									count_main--;
									continue;
								}
								temp = temp->next;
							}

							X = X->prev;
							temp = X->next;
							temp->prev->next = temp->next;
							temp->next->prev = temp->prev;
							delete temp;
							count_main--;
						}
					}
					// tien hanh duoi thuat su co energy >0
					else if (total_duong < total_chulinh) {
						customer* tmp = list_waiting_order->tail;
						while (tmp != nullptr) {
							if (tmp->energy > 0) {
								tmp->print();
							}
							tmp = tmp->prev;
						}
						tmp = list_waiting_order->head;
						while (tmp != nullptr) {
							if (tmp->energy > 0) {
								customer* bin = tmp;
								tmp = tmp->next;
								list_waiting->remove_cus(bin);
								list_waiting_order->remove_cus(bin);
								continue;
							}
							tmp = tmp->next;
						}
						//-------------------
						if (soluongduong > 0) {
							tmp = list_table->tail;
							while (tmp != nullptr) {
								if (tmp->energy > 0) {
									break;
								}
								tmp = tmp->prev;
							}
							while (true) {
								if (X->name == tmp->name) {
									break;
								}
								X = X->next;
							}
						}
						//___________
						tmp = list_table->tail;
						while (tmp != nullptr) {
							if (tmp->energy > 0) {
								customer* bin = tmp;
								bin->print();
								tmp = tmp->prev;
								list_table->remove_cus(bin);
								continue;
							}
							tmp = tmp->prev;
						}
						//_____________
						if (soluongduong > 0) {
							customer* temp = X->next;
							while (temp != X) {
								if (temp->energy > 0) {
									customer* bin = temp;
									temp = temp->next;
									bin->prev->next = bin->next;
									bin->next->prev = bin->prev;
									delete bin;
									count_main--;
									continue;
								}
								temp = temp->next;
							}
							X = X->next;
							temp = X->prev;
							temp->prev->next = temp->next;
							temp->next->prev = temp->prev;
							delete temp;
							count_main--;
						}
					}
				
				//tien hanh xep cho ngoi cho khach
				if (list_waiting->count > 0 && (MAXSIZE - list_table->count) >= list_waiting->count ) {
					int dem = list_waiting->count;
					for (int i = 0; i < dem; ++i) {
						used_BLUE = 1;
						RED(list_waiting->front()->name, list_waiting->front()->energy);
						list_waiting_order->remove_cus(list_waiting->front());
						list_waiting->dequeue();
					}
				}
				else if (list_waiting->count > 0 && (MAXSIZE - list_table->count) < list_waiting->count && (MAXSIZE - list_table->count) > 0) {
					int dem = MAXSIZE - list_table->count;
					for (int i = 0; i < dem; ++i) {
						used_BLUE = 1;
						RED(list_waiting->front()->name, list_waiting->front()->energy);
						list_waiting_order->remove_cus(list_waiting->front());
						list_waiting->dequeue();
					}
				}
			}
		}

		void LIGHT(int num)
		{
			if (num > 0) {
				if (X != nullptr) {
					X->print();
					if (list_table->count > 1) {
						customer* tmp = X->next;
						while (tmp != X) {
							tmp->print();
							tmp = tmp->next;
						}
					}
				}
			}
			else if (num < 0) {
				if (X != NULL) {
					X->print();
					if (list_table->count > 1) {
						customer* tmp = X->prev;
						while (tmp != X) {
							tmp->print();
							tmp = tmp->prev;
						}
					}
				}
			}
			else {
				list_waiting->print_waiting();
			}
		}

		class queue {
		public:
			customer* head;
			customer* tail;
			int count;
		public:
			queue() {
				head = nullptr;
				tail = nullptr;
				count = 0;
			}
			~queue() {
				this->clear();
			}
			void enqueue(customer* cus) {
				customer* tmp = new customer(cus->name, cus->energy, nullptr, nullptr);
				if (count == 0) {
					head = tmp;
					tail = tmp;
					count++;
				}
				else {
					tail->next = tmp;
					tmp->prev = tail;
					tail = tmp;
					count++;
				}
			}
			void dequeue() {
				if (count > 0) {
					if (count == 1) {
						delete head;
						head = nullptr;
						tail = nullptr;
						count = 0;
					}
					else {
						customer* tmp = head;
						head = head->next;
						head->prev = nullptr;
						delete tmp;
						count--;
					}
				}
			}
			customer* front() {
				return head;
			}
			customer* rear() {
				return tail;
			}
			bool empty() {
				if (count == 0) return 1;
				return 0;
			}
			bool contain_name(string name) {
				if (count > 0) {
					customer* tmp = head;
					while (tmp != nullptr) {
						if (tmp->name == name) {
							return 1;
						}
						tmp = tmp->next;
					}
				}
				return 0;
			}
			void clear() {
				customer* tmp = head;
				while (head != tail) {
					head = head->next;
					delete tmp;
					tmp = head;
				}
				delete head;
				head = nullptr;
				tail = nullptr;
				count = 0;
			}
			void remove_cus(customer* cus) {
				customer* tmp = head;
				for (int i = 0; i < count; i++) {
					if (tmp->name == cus->name) {
						break;
					}
					tmp = tmp->next;
				}
				if (tmp->name == head->name) {
					dequeue();
				}
				else if (tmp->name == tail->name) {
					tail = tail->prev;
					tail->next = nullptr;
					--count;
					delete tmp;
					tmp = nullptr;
				}
				else {
					tmp->prev->next = tmp->next;
					tmp->next->prev = tmp->prev;
					delete tmp;
					tmp = nullptr;
					count--;
				}
			}
			void print_waiting() {
				customer* tmp = head;
				for (int i = 0; i < count; ++i) {
					tmp->print();
					tmp = tmp->next;
				}
			}
			customer* get(int index) {
				if (index < 0 || index >= count) {
					throw out_of_range("Out of range!");
				}
				if (index == 0) return head;
				else if (index == count - 1) return tail;
				else if (index <= count / 2) {
					customer* tmp = head;
					for (int i = 0; i < index; i++) {
						tmp = tmp->next;
					}
					return tmp;
				}
				else if (index > count / 2) {
					customer* tmp = tail;
					for (int i = 0; i < count - index - 1; i++) {
						tmp = tmp->prev;
					}
					return tmp;
				}
				return nullptr;
			}
			int index_name(string name) {
				customer* tmp = head;
				for (int i = 0; i < count; i++) {
					if (tmp->name == name) {
						return i;
					}
					tmp = tmp->next;
				}
				return -1;
			}

			int sumEnergy() {
				if (count > 0) {
					customer* tmp = head;
					int sum = 0;
					while (tmp != NULL) {
						sum += tmp->energy;
						tmp = tmp->next;
					}
					return sum;
				}
				return 0;
			}

			void copy(queue* myqueue) {
				this->clear();
				customer* tmp = myqueue->head;
				while (tmp != nullptr) {
					this->enqueue(tmp);
					tmp = tmp->next;
				}
			}
		};
		class stack {
		public:
			customer* head;
			customer* tail;
			int count;
		public:
			stack() {
				head = nullptr;
				tail = nullptr;
				count = 0;
			}
			~stack() {};
			void push(customer* cus) {
				customer* tmp = new customer(cus->name, cus->energy, nullptr, nullptr);
				if (count == 0) {
					head = tmp;
					tail = tmp;
					count = 1;
				}
				else {
					tail->next = tmp;
					tmp->prev = tail;
					tail = tail->next;
					count++;
				}
			}
			void pop() {
				if (count > 0) {
					if (count == 1) {
						delete head;
						head = nullptr;
						tail = nullptr;
						count = 0;
					}
					else {
						customer* tmp = tail;
						tail = tail->prev;
						tail->next = nullptr;
						delete tmp;
						count--;
					}
				}
			}
			customer* top() {
				return tail;
			}
			bool empty() {
				if (count == 0) return 1;
				return 0;
			}
		};
};