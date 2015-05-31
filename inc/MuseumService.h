#ifndef MUSEUMSERVISE_H
#define MUSEUMSERVISE_H

class MuseumService {
	private:
        int time;
		void startExposition();
		void endExposition();

	public:
		MuseumService();
		void run();
};

#endif
