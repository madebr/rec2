#!/usr/bin/env python

import argparse
import dataclasses
import io
import pathlib
import typing

import common


@dataclasses.dataclass
class FrontendItemSpec:
    id: int
    text: str
    x: int
    y: int
    width: int
    height: int
    unlit_font: int
    hilighted_font: int
    pixmap_index: int
    greyed_out_backdrop: int
    selectable: int
    group: int
    level_bar_id: int
    wrap_text: int
    disable_glow: int


@dataclasses.dataclass
class FrontendScrollerSpec:
    item_id: int
    count: int
    nb_displayed_at_once: int
    index_top_item: int
    index_first_scrollable_item: int
    index_last_scrollable_item: int
    greyed_out_backdrop: bool
    x_grey_area: int
    y_grey_area: int
    width_grey_area: int
    height_grey_area: int


@dataclasses.dataclass
class FrontendRadioSpec:
    item_id: int
    count: int
    index_first: int
    index_last: int
    index_selected: int
    greybox_requested: int
    x_greybox: int
    y_greybox: int
    width_greybox: int
    height_greybox: int


@dataclasses.dataclass
class FrontendLevelSpec:
    unknown0: int
    unknown1: int
    unknown2: int
    unknown3: int


@dataclasses.dataclass
class FrontendSpec:
    version: int
    name: str
    group_count: int
    backdrop: str
    items: list[FrontendItemSpec] = dataclasses.field(default_factory=list)
    scrollers: list[FrontendScrollerSpec] = dataclasses.field(default_factory=list)
    radios: list[FrontendRadioSpec] = dataclasses.field(default_factory=list)
    levels: list[FrontendLevelSpec] = dataclasses.field(default_factory=list)


def read_interface_spec(file: typing.IO) -> FrontendSpec:
    reader = common.TextReader(file)

    version_str, version_int_str, *_ = reader.read_line_parts()
    assert version_str == "VERSION"

    spec_version = int(version_int_str)
    assert spec_version == 3

    spec_name = reader.read_string()

    nbElems = reader.read_int()
    spec_groupCount = reader.read_int()
    spec_backdrop = reader.read_string()

    spec = FrontendSpec(version=spec_version, name=spec_name, group_count=spec_groupCount, backdrop=spec_backdrop)

    for i in range(nbElems):
        item_text = reader.read_string().replace("_", " ")
        item_x, item_y = reader.read_ints(2)
        item_width, item_height = reader.read_ints(2)
        item_unlitFont, item_hilightedFont = reader.read_ints(2)
        item_pixmapIndex = reader.read_int()
        item_greyedOutBackdrop = reader.read_int() != 0
        item_selectable = reader.read_int()
        item_group = reader.read_int()
        item_levelBarId = reader.read_int()
        item_wrapText = reader.read_int()
        item_disableGlow = reader.read_int()
        itemSpec = FrontendItemSpec(
            id=i,
            text=item_text,
            x=item_x,
            y=item_y,
            width=item_width,
            height=item_height,
            unlit_font=item_unlitFont,
            hilighted_font=item_hilightedFont,
            pixmap_index=item_pixmapIndex,
            greyed_out_backdrop=item_greyedOutBackdrop,
            selectable=item_selectable,
            group=item_group,
            level_bar_id=item_levelBarId,
            wrap_text=item_wrapText,
            disable_glow=item_disableGlow,
        )
        spec.items.append(itemSpec)

    nbScrollers = reader.read_int()
    for i in range(nbScrollers):
        scrollerSpec_itemId, scrollerSpec_count = reader.read_ints(2)
        scrollerSpec_nbDisplayedAtOnce, scrollerSpec_indexTopItem = reader.read_ints(2)
        scrollerSpec_indexFirstScrollableItem, scrollerSpec_indexLastScrollableItem = reader.read_ints(2)
        scrollerSpec_greyedOutBackdrop = reader.read_int()
        scrollerSpec_x_greyArea, scrollerSpec_y_greyArea = reader.read_ints(2)
        scrollerSpec_width_greyArea, scrollerSpec_height_greyArea = reader.read_ints(2)
        scrollerSpec = FrontendScrollerSpec(
            item_id=scrollerSpec_itemId,
            count=scrollerSpec_count,
            nb_displayed_at_once=scrollerSpec_nbDisplayedAtOnce,
            index_top_item=scrollerSpec_indexTopItem,
            index_first_scrollable_item=scrollerSpec_indexFirstScrollableItem,
            index_last_scrollable_item=scrollerSpec_indexLastScrollableItem,
            greyed_out_backdrop=scrollerSpec_greyedOutBackdrop,
            x_grey_area=scrollerSpec_x_greyArea,
            y_grey_area=scrollerSpec_y_greyArea,
            width_grey_area=scrollerSpec_width_greyArea,
            height_grey_area=scrollerSpec_height_greyArea,
        )
        spec.scrollers.append(scrollerSpec)

    nbradios = reader.read_int()
    for i in range(nbradios):
        radioSpec_itemId, radioSpec_count = reader.read_ints(2)
        radioSpec_indexFirst, radioSpec_indexLast = reader.read_ints(2)
        radioSpec_indexSelected, radioSpec_greyboxRequested = reader.read_ints(2)
        radioSpec_x_greybox, radioSpec_y_greybox = reader.read_ints(2)
        radioSpec_width_greybox, radioSpec_height_greybox = reader.read_ints(2)
        radioSpec = FrontendRadioSpec(
            item_id=radioSpec_itemId,
            count=radioSpec_count,
            index_first=radioSpec_indexFirst,
            index_last=radioSpec_indexLast,
            index_selected=radioSpec_indexSelected,
            greybox_requested=radioSpec_greyboxRequested,
            x_greybox=radioSpec_x_greybox,
            y_greybox=radioSpec_y_greybox,
            width_greybox=radioSpec_width_greybox,
            height_greybox=radioSpec_height_greybox,
        )
        spec.radios.append(radioSpec)

    nbLevels = reader.read_int()
    for i in range(nbLevels):
        levelSpec_unknown0, levelSpec_unknown1 = reader.read_ints(2)
        levelSpec_unknown2, levelSpec_unknown3 = reader.read_ints(2)
        levelSpec = FrontendLevelSpec(
            unknown0=levelSpec_unknown0,
            unknown1=levelSpec_unknown1,
            unknown2=levelSpec_unknown2,
            unknown3=levelSpec_unknown3,
        )
        spec.levels.append(levelSpec)

    # Not parsed by the executable
    try:
        nbBackdrops = reader.read_int()
    except EOFError:
        nbBackdrops = 0

    for i in range(nbBackdrops):
        reader.read_int()

    try:
        reader.read_line()
        assert False
    except EOFError:
        pass

    return spec


def main():
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("file", metavar="FILE", default=None, type=pathlib.Path, nargs="?", help="file to parse")

    args = parser.parse_args()

    f = common.open_data_file(args.file)
    spec = read_interface_spec(f)

    d = dataclasses.asdict(spec)
    import yaml
    import sys
    yaml.safe_dump(d, sys.stdout)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
