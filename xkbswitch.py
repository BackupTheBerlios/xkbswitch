
import _xkbswitch

class XkbSwitch(object):
    def __init__(self, display_name=None):
        _xkbswitch.initialize(display_name)
        self._layouts = _xkbswitch.get_layouts()
        self._layouts_usage = {}.fromkeys(self._layouts, 1)
        self._layouts += [None] * ((_xkbswitch.get_max_num_groups() or 8) -
            len(self._layouts))
        #self.current_layout = self._layouts[_xkbswitch.get_group()]
        self.current_layout = None

    def __del__(self):
        _xkbswitch.finalize()

    def activate_layout(self, layout_name):
        try:
            _xkbswitch.set_group(self._layouts.index(layout_name))
            self._layouts_usage[layout_name] += 1
            self.current_layout = layout_name
        except ValueError:
            group = None
            if self._layouts.count(None) != 0:
                group = self._layouts.index(None)
            else:
                def filtered_usage():
                    return filter(lambda item: item[0] in self._layouts,
                        self._layouts_usage.items())
                key = min(((v, k) for (k, v) in filtered_usage()))[1]
                group = self._layouts.index(key)
            new_layouts = self._layouts
            new_layouts[group] = layout_name
            _xkbswitch.set_layouts(new_layouts)
            _xkbswitch.set_group(group)
            self._layouts = new_layouts
            self._layouts_usage[layout_name] = \
                self._layouts_usage.setdefault(layout_name, 0) + 1
            self.current_layout = layout_name


if __name__ == "__main__":
    _xkbswitch.initialize(None)
    print "Group:", _xkbswitch.get_group()
    print "Num groups:", _xkbswitch.get_num_groups()
    print "Max num groups:", _xkbswitch.get_max_num_groups()
    print "Layouts:", _xkbswitch.get_layouts()
    _xkbswitch.finalize()
