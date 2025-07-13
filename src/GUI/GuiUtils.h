#pragma once 

/**
 * Macro that triggers the display of a hidden tab bar for a specified ImGui dock node.
 *
 * This is useful when you want to programmatically toggle the visibility of the tab bar
 * in a docking space created by ImGui's DockBuilder API. It sets the internal flag
 * `WantHiddenTabBarToggle` to true if the node is marked for a hidden tab bar update.
 *
 * @param dock_id The ID of the ImGui dock node to modify.
 */
#define SHOW_HIDDEN_TAB_BAR(dock_id) \
    do { \
        if ((dock_id) != 0) { \
            if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id)) { \
                if (node->WantHiddenTabBarUpdate) node->WantHiddenTabBarToggle = true; \
            } \
        } \
    } while (0)




