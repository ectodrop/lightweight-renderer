#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include <rendering/scene.h>
class PerformanceDialogComponent : public UIComponent {
	using UIComponent::UIComponent;
public:
	void Render() override {
		if (_p_open) {
			if (!ImGui::Begin("Profile", &_p_open)) {
				ImGui::End();
			}
			else {
				float d_time = _prev_frame_time;
				if (!_profile_paused) {
					d_time = GetUIVar<float>("delta_time");
					_time_accumulator += d_time;
					_num_frame_samples++;
					if (_time_accumulator >= 1000) {
						_avg_last_second_frametime = _time_accumulator / _num_frame_samples;
						_num_frame_samples = 0;
						_time_accumulator = 0;
					}
				}
				ImGui::Text("frame duration: %f ms\n", d_time);
				ImGui::Text("avg frame duration (last 1000ms): %f ms\n", _avg_last_second_frametime);
				ImGui::Text("fps: %f fps\n", 1000 / d_time);
				ImGui::Text("avg fps (last 1000ms): %f fps\n", 1000 / _avg_last_second_frametime);
				ImGui::Text("compute workgroup count: (%f,%f,1)\n", GetUIVar<Scene*>("main_scene")->GetCamera()->width, GetUIVar<Scene*>("main_scene")->GetCamera()->height);

				if (ImGui::Button("toggle pause profile")) {
					_prev_frame_time = d_time;
					_profile_paused = !_profile_paused;
				}
				ImGui::End();
			}
		}
	}
private:
	bool _p_open = true, _profile_paused = false;
	float _prev_frame_time = 0;
	float _num_frame_samples = 0, _time_accumulator = 0, _avg_last_second_frametime;

};