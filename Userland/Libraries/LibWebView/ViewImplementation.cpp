/*
 * Copyright (c) 2023, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Error.h>
#include <AK/String.h>
#include <LibWebView/ViewImplementation.h>

namespace WebView {

WebContentClient& ViewImplementation::client()
{
    VERIFY(m_client_state.client);
    return *m_client_state.client;
}

WebContentClient const& ViewImplementation::client() const
{
    VERIFY(m_client_state.client);
    return *m_client_state.client;
}

void ViewImplementation::zoom_in()
{
    if (m_zoom_level >= ZOOM_MAX_LEVEL)
        return;
    m_zoom_level += ZOOM_STEP;
    update_zoom();
}

void ViewImplementation::zoom_out()
{
    if (m_zoom_level <= ZOOM_MIN_LEVEL)
        return;
    m_zoom_level -= ZOOM_STEP;
    update_zoom();
}

void ViewImplementation::reset_zoom()
{
    m_zoom_level = 1.0f;
    update_zoom();
}

void ViewImplementation::get_source()
{
    client().async_get_source();
}

void ViewImplementation::inspect_dom_tree()
{
    client().async_inspect_dom_tree();
}

void ViewImplementation::inspect_accessibility_tree()
{
    client().async_inspect_accessibility_tree();
}

ErrorOr<ViewImplementation::DOMNodeProperties> ViewImplementation::inspect_dom_node(i32 node_id, Optional<Web::CSS::Selector::PseudoElement> pseudo_element)
{
    auto response = client().inspect_dom_node(node_id, pseudo_element);
    if (!response.has_style())
        return Error::from_string_view("Inspected node returned no style"sv);
    return DOMNodeProperties {
        .computed_style_json = TRY(String::from_deprecated_string(response.take_computed_style())),
        .resolved_style_json = TRY(String::from_deprecated_string(response.take_resolved_style())),
        .custom_properties_json = TRY(String::from_deprecated_string(response.take_custom_properties())),
        .node_box_sizing_json = TRY(String::from_deprecated_string(response.take_node_box_sizing())),
    };
}

void ViewImplementation::clear_inspected_dom_node()
{
    client().inspect_dom_node(0, {});
}

i32 ViewImplementation::get_hovered_node_id()
{
    return client().get_hovered_node_id();
}

}
