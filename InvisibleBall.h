#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "NetcodeManager/NetcodeManager.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class InvisibleBall: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{
private:
    std::shared_ptr<NetcodeManager> Netcode;

public:
	//std::shared_ptr<bool> enabled;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	ServerWrapper GetCurrentGameState();

	void hookEvents();
	void unhookEvents();
	void onTick();
	void render(CanvasWrapper canvas);
	void flashOn();
	void flashOff();
	void updateClientInfo();
public:
	struct VisibilityData {
		std::deque<std::deque<bool>> invisDeque;
	};

	void OnMessageReceived(const std::string& Message, PriWrapper Sender);
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	void updatePlayerVisibleStates(VisibilityData newData);
	

	// Teleport ball example from CinderBlock's example plugin
	// https://github.com/CinderBlocc/NetcodeManagerExample
	// https://youtu.be/jyFuN8k2uIs
	// void RequestBall();

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "InvisibleBall";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/
};

