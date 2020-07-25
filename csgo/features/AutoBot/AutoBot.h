#pragma once

class c_autobot {
private:
	std::pair<float, float> ComputeMove(c_user_cmd* cmd, const vec3_t& a, const vec3_t& b);
	void WalkTo(const vec3_t& vector);
	bool navTo(const vec3_t& destination, bool should_repath);
	void repath();
	void ResetPather();
	void clearInstructions();
	void updateAreaScore();
	nav_mesh::nav_area* findClosestNavSquare(const vec3_t& vec);
	void Begin();
	void init();
	void Move();

public:
	void DrawPath();
	void run();
};
extern c_autobot g_autobot;