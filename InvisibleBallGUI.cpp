#include "pch.h"
#include "InvisibleBall.h"
#include "json.hpp"

using json = nlohmann::json;

std::string InvisibleBall::GetPluginName() {
	return "Invisible Ball";
}

void InvisibleBall::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InvisibleBall::VisibilityData, invisDeque);

void InvisibleBall::updatePlayerVisibleStates(VisibilityData newData)
{
	CVarWrapper invisStateCVar = cvarManager->getCvar("invis_ball_json");
	json invisJson = newData;
	invisStateCVar.setValue(invisJson.dump());
}


void InvisibleBall::RenderSettings() {
	CVarWrapper enableCvar = cvarManager->getCvar("invisible_ball_enabled");

	if (!enableCvar) {
		return;
	}

	bool enabled = enableCvar.getBoolValue();

	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle having invisible opponents");
	}

	// convert a cvar string like "1,Player 1;2,Player 2;3;4;5,Player 3;6;7;8" into a nice array

	CVarWrapper namesCvar = cvarManager->getCvar("invisibleball_names_specshortcuts");

	if (!namesCvar) return;

	std::string nameString = namesCvar.getStringValue();

	std::string shortcutsSeparated[8];

	std::string delimiter = ";";

	size_t pos = 0;
	std::string token;
	for (int i = 0; i < 7; i++) {
		pos = nameString.find(delimiter);
		if (pos == std::string::npos) break;
		token = nameString.substr(0, pos);
		shortcutsSeparated[i] = (token);
		nameString.erase(0, pos + delimiter.length());
	}
	// now nameString just contains the last token, so add it to the array
	shortcutsSeparated[7] = nameString;

	//now, go from things like "1,Player 1" or "3" to nice values
	std::string shortcutsToNames[9] = { "" };

	int specShortcut;

	for (int i = 0; i < 8; i++) {
		token = shortcutsSeparated[i];
		std::size_t commaPos = token.find(',');
		if (commaPos == std::string::npos) {
			// ',' isn't in the token, so this player doesn't exist
			// get the spectator shortcut
			try {
				specShortcut = std::stoi(token);
			}
			catch (const std::invalid_argument& e) {
				//we could not convert to an int. Not sure what to do here besides not crash, so...
				continue;
			}
			shortcutsToNames[specShortcut] = "";
		}
		else {
			// this player exists
			// get the spectator shortcut
			try {
				specShortcut = std::stoi(token.substr(0, commaPos));
			}
			catch (const std::invalid_argument& e) {
				//we could not convert to an int. Not sure what to do here besides not crash, so...
				continue;
			}
			// store the player name
			shortcutsToNames[specShortcut] = token.substr(commaPos + 1);
		}
	}
	// an array to keep track of player states
	static VisibilityData playerInvisibilityStates{ {{true, true, true}, {true, true, true}, {true, true, true}, {true, true, true}, {true, true, true}, {true, true, true}, {true, true, true}, {true, true, true}, {true, true, true}} };
	ImGui::TextUnformatted("Ball Flash Controls");
	CVarWrapper flashOffTimeCvar = cvarManager->getCvar("flash_off_time");
	if (flashOffTimeCvar) {
		float flashOffTime = flashOffTimeCvar.getFloatValue();
		if (ImGui::SliderFloat("Ball Off Time", &flashOffTime, 0.0, 20.0)) {
			flashOffTimeCvar.setValue(flashOffTime);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "flashOffTime is " + std::to_string(flashOffTime);
			ImGui::SetTooltip(hoverText.c_str());
		}
	}
	CVarWrapper flashOnTimeCvar = cvarManager->getCvar("flash_on_time");
	if (flashOnTimeCvar) {
		float flashOnTime = flashOnTimeCvar.getFloatValue();
		if (ImGui::SliderFloat("Ball Off Time", &flashOnTime, 0.0, 20.0)) {
			flashOnTimeCvar.setValue(flashOnTime);
		}
		if (ImGui::IsItemHovered()) {
			std::string hoverText = "flashOffTime is " + std::to_string(flashOnTime);
			ImGui::SetTooltip(hoverText.c_str());
		}
	}
	ImGui::TextUnformatted("Ball Visibility Controls");
	CVarWrapper hostIsSpectating = cvarManager->getCvar("host_is_spectator");
	// taking advantage of short-circuit operators here:
	// if hostIsSpectating is not null AND hostIsSpectating
	if (hostIsSpectating && hostIsSpectating.getBoolValue()) {
		CVarWrapper hostCanSeeBall = cvarManager->getCvar("host_can_see_ball");
		if (!hostCanSeeBall) {
			ImGui::TextUnformatted("Something has gone wrong and you are unable to set whether the host can see the ball.");
		}
		else {
			bool hostCanSeeBallBool = hostCanSeeBall.getBoolValue();
			if (ImGui::Checkbox("Host (you) can see ball", &hostCanSeeBallBool)) {
				hostCanSeeBall.setValue(hostCanSeeBallBool);
			}
		}
	}
	// make a tree to display options for each player
	std::string playerName;
	for (int i = 1; i < 9; i++) {
		playerName = shortcutsToNames[i];
		if (playerName == std::string("")) {
			continue;
		}
		if (ImGui::TreeNode(playerName.c_str())) {
			if (ImGui::Checkbox("Can See Ball", &playerInvisibilityStates.invisDeque[i][0])) {
				updatePlayerVisibleStates(playerInvisibilityStates);
			}
			if (ImGui::Checkbox("Has Arrow To Ball", &playerInvisibilityStates.invisDeque[i][1])) {
				updatePlayerVisibleStates(playerInvisibilityStates);
			}
			if (ImGui::Checkbox("Ball Is Flashing", &playerInvisibilityStates.invisDeque[i][2])) {
				updatePlayerVisibleStates(playerInvisibilityStates);
			}
			ImGui::TreePop();
		}
	}

	ImGui::Separator();

	ImGui::TextUnformatted("Plugin made by rivques#4310 - DM me on discord for custom plugin commissions!");
}

/*
// Do ImGui rendering here
void InvisibleBall::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string InvisibleBall::GetMenuName()
{
	return "InvisibleBall";
}

// Title to give the menu
std::string InvisibleBall::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void InvisibleBall::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool InvisibleBall::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool InvisibleBall::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void InvisibleBall::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void InvisibleBall::OnClose()
{
	isWindowOpen_ = false;
}
*/
