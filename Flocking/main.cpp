#include <SDL.h>
#include <graphics.h>
#include <list>
#include <thread>
#include <chrono>

#include "Agent.h"

#define SQ(x) x*x 

#define MAX_VEL 4
#define MAX_FORCE 0.1

#define NEIGHBOURHOOD 100
#define PERSONAL_SPACE 70
#define CATCH_ZONE 5

#define W 1000
#define H 600

Vector2D& limit(Vector2D& v, double r) {
	if (v.magnitudeSq() > r*r) {
		v.normalise();
		v *= r;
	}
	return v;
}

int pair(Agent& a, Agent& b, Vector2D& pc, Vector2D& rp, Vector2D& pv) {
	Vector2D rel = b.pos - a.pos;
	double dist = rel.magnitudeSq();
	rel.normalise();
	if (dist < SQ(NEIGHBOURHOOD) && dist > 0) {
		if (a.type == b.type) {
			pc += b.pos;
			pv += b.vel;
			if (dist < SQ(PERSONAL_SPACE)) {
				rp += rel;// / dist;
			}

			return 0; // Flocking iteration
		}
		
		limit(rel, MAX_FORCE * 5);
		if (a.type == Agent::NONE) { // && b.type == Agent::PREDATOR
			a.tmpvel -= rel;
		}
		else { // a.type == Agent::PREDATOR && b.type == Agent::NONE
			a.tmpvel += rel;
			if (dist < SQ(CATCH_ZONE)) {
				return -2; // Caught prey
			}
		}
	}

	return -1; // No interaction
}

int main(int argv, char *argc[]) {
	GraphicsManager gm;
	if (!gm.init("Flocking", AARect(Vector2D(50, 50), Vector2D(W, H)), NULL)) {
		return -1;
	}

	Database flock;
	glLineWidth(3);

	bool quit = false;
	while (!quit) {
		flock.sort();

		// Simulate
		for (nodeiter<Agent*> a = flock.xbegin(); !a.done(); ++a) {
			(*a)->tmpvel = (*a)->vel;
			Vector2D pc, rp, pv; // percieved centre, repulse centre, percieved velocity
			unsigned int sum = 0;

			// Left - left is covered by others
			for (nodeiter<Agent*> l = a.prev(); !l.rdone(); --l) {
				// If x distance is out of range, so is the total
				if ((*l)->pos[0][0] - (*a)->pos[0][0] > NEIGHBOURHOOD) break;

				// If not in y range, ignore
				if ((*l)->pos[1][0] - (*a)->pos[1][0] > NEIGHBOURHOOD) continue;

				// AI logic
				switch (pair(**a, **l, pc, rp, pv)) {
					case 0:
						++sum;
						break;
					case -2:
						for (nodeiter<Agent> i = flock.begin(); !i.done(); ++i) {
							if (&(*i) == *l) {
								i.remove();
								break;
							}
						}
						l.remove();
						break;
				}
			}

			// Right
			for (nodeiter<Agent*> r = a.next(); !r.done(); ++r) {
				// If x distance is out of range, so is the total
				if ((*r)->pos[0][0] - (*a)->pos[0][0] > NEIGHBOURHOOD) break;

				// If not in y range, ignore
				if ((*r)->pos[1][0] - (*a)->pos[1][0] > NEIGHBOURHOOD) continue;

				// AI logic
				if (pair(**a, **r, pc, rp, pv) == 0) ++sum;
			}

			if (sum > 0) {
				// Cohesion
				pc /= sum;
				pc -= (*a)->pos;
				limit(pc, MAX_VEL);
				pc -= (*a)->vel;
				(*a)->tmpvel += limit(pc, MAX_FORCE);

				// Seperation
				rp /= sum;
				(*a)->tmpvel -= limit(rp, MAX_FORCE * 1.5);

				// Alignment
				pv /= sum;
				pv -= (*a)->vel;
				(*a)->tmpvel += limit(pv, MAX_FORCE);
			}

			// FULL POWER!!
			//(*a)->tmpvel.normalise();
			//(*a)->tmpvel *= MAX_VEL;
			if ((*a)->tmpvel.magnitudeSq() > MAX_VEL * MAX_VEL) {
				(*a)->tmpvel.normalise();
				(*a)->tmpvel *= MAX_VEL;
			}
			if ((*a)->type == Agent::PREDATOR) (*a)->tmpvel *= 0.9;
		}

		for (nodeiter<Agent> i = flock.begin(); !i.done(); ++i) {
			i->vel = i->tmpvel;
			i->pos += i->vel;

			// Keep on screen
			i->pos[0][0] = fmod(i->pos[0][0], W);
			if (i->pos[0][0] < 0) i->pos[0][0] += W;
			i->pos[1][0] = fmod(i->pos[1][0], H);
			if (i->pos[1][0] < 0) i->pos[1][0] += H;
		}

		// Render
		gm.rendererClear(WHITE);
		gm.setRendererDrawColour(BLACK);
		// Draw
		for (nodeiter<Agent> i = flock.begin(); !i.done(); ++i) {
			Vector2D forward = i->vel;
			forward.normalise();
			forward *= 10;
			Vector2D vec[] = {i->pos, i->pos + forward};
			if (i->type == Agent::PREDATOR) {
				gm.setRendererDrawColour(RED);
				gm.plot(vec, 2);
				gm.setRendererDrawColour(BLACK);
			}
			else gm.plot(vec, 2);
		}
		gm.render();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
				{
					double a = (float)rand() / RAND_MAX * M_PI * 2;
					Agent agent(Vector2D(e.button.x, e.button.y), Vector2D(cos(a), sin(a)) * MAX_VEL);
					if (e.button.button == SDL_BUTTON_RIGHT) agent.type = Agent::PREDATOR;
					flock.add(agent);
				}
				break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_f:
						{
							int num = 60;
							if (e.key.keysym.mod & KMOD_SHIFT) num /= 6;
							for (int i = 0; i < num; ++i) {
								double a = (double)rand() / RAND_MAX * 2 * M_PI;
								Agent agent(Vector2D(W / 2, H / 2), Vector2D(cos(a), sin(a)) * MAX_VEL);
								if (e.key.keysym.mod & KMOD_CTRL) agent.type = Agent::PREDATOR;
								flock.add(agent);
							}
						}
						break;
						case SDLK_r:
							flock.clear();
							break;
					}
					break;
			}
		}
	}
}