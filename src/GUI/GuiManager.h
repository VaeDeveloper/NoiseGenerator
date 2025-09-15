#pragma once

#include <imgui_internal.h>
#include <imgui.h>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>
#include <mutex>


#include "NoisePropertyUI.h"
#include "NoisePreviewPanelUI.h"

// todo 
#include "MenuBarUI.h"

/**
 * Class responsible for managing the graphical user interface (GUI) using ImGui.
 * Encapsulates initialization, rendering, and UI layout for a noise-based editor or viewer.
 */
class GuiManager
{
public:
	/**
	 * Initializes the ImGui context and binds it to the provided GLFW window.
	 *
	 * @param window Pointer to the GLFW window used for input and context.
	 */
	void Initialize(GLFWwindow* window);

	/**
	 * Cleans up ImGui resources and shuts down the GUI system.
	 */
	void Shutdown();

	/**
	 * Begins a new ImGui frame. Should be called once per frame before drawing any UI.
	 */
	void BeginFrame();

	/**
	 * Renders the current ImGui frame. Should be called after all UI drawing is done.
	 */
	void Render();

	/**
	 * Draws all custom GUI panels and windows.
	 */
	void DrawUI();

	/**
	 * Supplies noise data to the GUI for preview or editing.
	 *
	 * @param data Pointer to the float array containing noise values.
	 * @param width The width of the noise data grid.
	 * @param height The height of the noise data grid.
	 */
	void SetNoiseData(float* data, int width, int height);

private:
	/**
	 * Configures the ImGui style (e.g., padding, rounding).
	 *
	 * @param io Reference to the ImGuiIO structure.
	 */
	void InitStyleConfig(const ImGuiIO& io);

	/**
	 * Loads and configures fonts for ImGui.
	 *
	 * @param io Reference to the ImGuiIO structure.
	 */
	void InitFontConfig(const ImGuiIO& io);

	/**
	 * Applies a custom theme/style to ImGui components.
	 */
	void InitThemeStyle();

	/**
	 * Draws the main menu bar at the top of the window.
	 */
	void DrawMenuBar();

	/**
	 * Draws the dock space and handles docking layout logic.
	 */
	void DrawDockTab();

	/**
	 * Draws the preview panel for displaying noise output.
	 */
	void DrawPreviewPanel();

	/**
	 * Draws the property panel for modifying noise settings.
	 */
	void DrawPropertyPanel();

	/**
	 * Draws the output log panel for displaying messages and logs.
	 */
	void DrawOutputLog();

	/**
	 * Handles the visibility state of the info panel.
	 *
	 * @param visible True to show the info panel, false to hide it.
	 */
	void InfoPanelVisibleHandle(bool visible);

private:
	/**
	 * Flag indicating whether the docking layout has been built.
	 */
	bool bDockBuilt = false;

	/**
	 * Flag indicating whether the output log panel should be shown.
	 */
	bool bShowOutputLog = true;

	/**
	 * UI component representing the main menu bar.
	 */
	MenuBarUI menuBar;

	/**
	 * UI component for adjusting noise generation parameters.
	 */
	NoisePropertyUI propertyUI;

	/**
	 * UI component for displaying the noise preview.
	 */
	NoisePreviewPanelUI noisePreview;
};
