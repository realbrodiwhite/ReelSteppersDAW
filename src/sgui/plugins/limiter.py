# -*- coding: utf-8 -*-
"""
This file is part of the Stargate project, Copyright Stargate Team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

"""

from sgui.widgets import *
from sglib.lib.translate import _


SG_LIM_THRESHOLD = 0
SG_LIM_CEILING = 1
SG_LIM_RELEASE = 2
SG_LIM_UI_MSG_ENABLED = 3


SG_LIM_PORT_MAP = {}

STYLESHEET = """\
QWidget#plugin_window{
    background: qlineargradient(
        x1: 0, y1: 0, x2: 1, y2: 1,
        stop: 0 #11116a, stop: 0.5 #1c1c5f, stop: 1 #11116a
    );
}

QComboBox{
    background: qlineargradient(
        x1: 0, y1: 0, x2: 0, y2: 1,
        stop: 0 #6a6a6a, stop: 0.5 #828282, stop: 1 #6a6a6a
    );
    border: 1px solid #222222;
    border-radius: 6px;
    color: #cccccc;
}

QLabel#plugin_name_label,
QLabel#plugin_value_label{
    background: none;
    color: #cccccc;
}
"""


class LimiterPluginUI(AbstractPluginUI):
    def __init__(self, *args, **kwargs):
        AbstractPluginUI.__init__(
            self,
            *args,
            stylesheet=STYLESHEET,
            **kwargs,
        )
        self._plugin_name = "SG Limiter"
        self.is_instrument = False

        self.preset_manager = None
        self.layout.setSizeConstraint(
            QLayout.SizeConstraint.SetFixedSize,
        )

        self.delay_hlayout = QHBoxLayout()
        self.layout.addLayout(self.delay_hlayout)

        f_knob_size = DEFAULT_LARGE_KNOB_SIZE

        self.groupbox_gridlayout = QGridLayout()
        self.delay_hlayout.addLayout(self.groupbox_gridlayout)

        knob_kwargs={
            'arc_brush': QColor("#cccccc"),
            'arc_width_pct': 12.,
            'fg_svg': os.path.join(
                util.PLUGIN_ASSETS_DIR,
                'knob-metal-3.svg',
            ),
        }

        self.thresh_knob = knob_control(
            f_knob_size,
            _("Thresh"),
            SG_LIM_THRESHOLD,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            -360,
            0,
            0,
            KC_TENTH,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.thresh_knob.add_to_grid_layout(self.groupbox_gridlayout, 3)

        self.ceiling_knob = knob_control(
            f_knob_size,
            _("Ceiling"),
            SG_LIM_CEILING,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            -180,
            0,
            0,
            KC_TENTH,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.ceiling_knob.add_to_grid_layout(self.groupbox_gridlayout, 7)

        self.release_knob = knob_control(
            f_knob_size,
            _("Release"),
            SG_LIM_RELEASE,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            50,
            1500,
            500,
            KC_INTEGER,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.release_knob.add_to_grid_layout(self.groupbox_gridlayout, 22)

        self.peak_meter = peak_meter(16, False)
        self.delay_hlayout.addWidget(self.peak_meter.widget)

        self.ui_msg_enabled = null_control(
            SG_LIM_UI_MSG_ENABLED,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            0,
            self.port_dict,
        )

        self.open_plugin_file()
        self.set_midi_learn(SG_LIM_PORT_MAP)
        self.enable_ui_msg(True)

    def widget_close(self):
        self.enable_ui_msg(False)
        AbstractPluginUI.widget_close(self)

    def widget_show(self):
        self.enable_ui_msg(True)

    def enable_ui_msg(self, a_enabled):
        if a_enabled:
            self.plugin_val_callback(
                SG_LIM_UI_MSG_ENABLED, 1.0)
        else:
            self.plugin_val_callback(
                SG_LIM_UI_MSG_ENABLED, 0.0)

    def ui_message(self, a_name, a_value):
        if a_name == "gain":
            self.peak_meter.set_value([a_value] * 2)
        else:
            AbstractPluginUI.ui_message(a_name, a_value)

    def save_plugin_file(self):
        # Don't allow the peak meter to run at startup
        self.ui_msg_enabled.set_value(0)
        AbstractPluginUI.save_plugin_file(self)

