/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef THEMEPALETTEDATA_H
#define THEMEPALETTEDATA_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QColor>

struct ThemePalette {
    const char *id;
    const char *displayName;
    const char *family;
    bool isDark;
    const char *win;     // Window Background
    const char *card;    // Editor Canvas Background
    const char *border;  // Margin / Line numbers / Fold margin
    const char *txtPri;  // Primary text / Default text
    const char *txtSec;  // Secondary text / Comments
    const char *cyan;    // Keywords / Types / Tags
    const char *blue;    // Functions / Methods / Classes
    const char *net;     // Preprocessor / Accents
    const char *sub;     // Numbers / Constants
    const char *host;    // Strings / Literals
    const char *sep;     // Operators / Delimiters
};

inline const ThemePalette kAllThemePalettes[] = {
    {"notepadNext", "NotepadNext Default", "NotepadNext", false, "#f0f2f5", "#fafbfc", "#e1e4e8", "#24292f", "#57606a",
     "#0969da", "#0550ae", "#cf222e", "#0550ae", "#1a7f37", "#24292f"},
    {"catppuccinMocha", "Catppuccin Mocha", "Catppuccin", true, "#11111b", "#1e1e2e", "#313244", "#cdd6f4", "#a6adc8",
     "#74c7ec", "#89b4fa", "#74c7ec", "#cba6f7", "#a6e3a1", "#6c7086"},
    {"catppuccinMacchiato", "Catppuccin Macchiato", "Catppuccin", true, "#181926", "#24273a", "#363a4f", "#cad3f5",
     "#a5adcb", "#7dc4e4", "#8aadf4", "#7dc4e4", "#c6a0f6", "#a6da95", "#6e738d"},
    {"catppuccinFrappe", "Catppuccin Frappé", "Catppuccin", true, "#232634", "#303446", "#414559", "#c6d0f5", "#a5adce",
     "#85c1dc", "#8caaee", "#85c1dc", "#ca9ee6", "#a6d189", "#737994"},
    {"catppuccinLatte", "Catppuccin Latte", "Catppuccin", false, "#dce0e8", "#eff1f5", "#ccd0da", "#4c4f69", "#6c6f85",
     "#1e66f5", "#1e66f5", "#209fb5", "#8839ef", "#40a02b", "#9ca0b0"},
    {"dracula", "Dracula", "Dracula", true, "#1e1f29", "#282a36", "#44475a", "#f8f8f2", "#8897c6", "#8be9fd", "#bd93f9",
     "#8be9fd", "#bd93f9", "#50fa7b", "#6272a4"},
    {"draculaSoft", "Dracula Soft", "Dracula", true, "#21222d", "#282a36", "#4d5166", "#f8f8f2", "#7887b4", "#94ebfc",
     "#c39dfc", "#94ebfc", "#c39dfc", "#64fc8b", "#7887b4"},
    {"draculaAlucard", "Dracula Alucard", "Dracula", true, "#15161e", "#1d1e28", "#373a4c", "#eaeaf0", "#5a668e",
     "#7ee5fa", "#b084f4", "#7ee5fa", "#b084f4", "#42f06f", "#5a668e"},
    {"draculaLight", "Dracula Light", "Dracula", false, "#f8f8f2", "#ffffff", "#e2e8f0", "#282a36", "#6272a4",
     "#0097a7", "#7c4dff", "#0097a7", "#7c4dff", "#2e7d32", "#6272a4"},
    {"tokyoNight", "Tokyo Night", "Tokyo Night", true, "#16161e", "#1a1b26", "#2f334d", "#c0caf5", "#9aa5ce", "#7dcfff",
     "#7aa2f7", "#2ac3de", "#bb9af7", "#9ece6a", "#565f89"},
    {"tokyoNightStorm", "Tokyo Night Storm", "Tokyo Night", true, "#1f2335", "#24283b", "#3b4261", "#c0caf5", "#9aa5ce",
     "#7dcfff", "#7aa2f7", "#2ac3de", "#bb9af7", "#9ece6a", "#565f89"},
    {"tokyoNightLight", "Tokyo Night Light", "Tokyo Night", false, "#d5d6db", "#e1e2e7", "#cfc9c2", "#343b58",
     "#565a6e", "#0f4b6e", "#2e7de9", "#007197", "#9854f1", "#587539", "#8990b3"},
    {"nordDark", "Nord Dark", "Nord", true, "#242933", "#2e3440", "#3b4252", "#d8dee9", "#88c0d0", "#88c0d0", "#81a1c1",
     "#8fbcbb", "#b48ead", "#a3be8c", "#4c566a"},
    {"nordPolar", "Nord Polar", "Nord", true, "#1d212a", "#242933", "#3b4252", "#e5e9f0", "#81a1c1", "#88c0d0",
     "#5e81ac", "#88c0d0", "#b48ead", "#a3be8c", "#4c566a"},
    {"nordLight", "Nord Light", "Nord", false, "#e5e9f0", "#eceff4", "#d8dee9", "#2e3440", "#4c566a", "#5e81ac",
     "#81a1c1", "#3b626e", "#83587a", "#4c704c", "#9da6b8"},
    {"oneDarkPro", "One Dark Pro", "One Dark", true, "#21252b", "#282c34", "#3e4451", "#dcdfe4", "#828997", "#56b6c2",
     "#61afef", "#56b6c2", "#c678dd", "#98c379", "#4b5263"},
    {"oneDarkVivid", "One Dark Vivid", "One Dark", true, "#1e2227", "#282c34", "#4b5263", "#e5e5e5", "#7f848e",
     "#4dc4d4", "#528bff", "#4dc4d4", "#d55fde", "#98c379", "#5c6370"},
    {"oneLight", "One Light", "One Dark", false, "#eaeaeb", "#fafafa", "#dbdbdc", "#383a42", "#696c77", "#0184bc",
     "#4078f2", "#0184bc", "#a626a4", "#50a14f", "#a0a1a7"},
    {"gruvboxDark", "Gruvbox Dark", "Gruvbox", true, "#1d2021", "#282828", "#3c3836", "#ebdbb2", "#a89984", "#83a598",
     "#458588", "#8ec07c", "#d3869b", "#b8bb26", "#928374"},
    {"gruvboxDarkMedium", "Gruvbox Dark Medium", "Gruvbox", true, "#282828", "#32302f", "#504945", "#ebdbb2", "#bdae93",
     "#83a598", "#458588", "#8ec07c", "#d3869b", "#b8bb26", "#928374"},
    {"gruvboxLight", "Gruvbox Light", "Gruvbox", false, "#f2e5bc", "#fbf1c7", "#ebdbb2", "#3c3836", "#7c6f64",
     "#076678", "#458588", "#427b58", "#8f3f71", "#79740e", "#928374"},
    {"solarizedDark", "Solarized Dark", "Solarized", true, "#00212b", "#002b36", "#073642", "#b5c2c4", "#7c9197",
     "#268bd2", "#268bd2", "#2aa198", "#6c71c4", "#859900", "#586e75"},
    {"solarizedLight", "Solarized Light", "Solarized", false, "#eee8d5", "#fdf6e3", "#e0d8c3", "#657b83", "#93a1a1",
     "#268bd2", "#268bd2", "#2aa198", "#6c71c4", "#859900", "#93a1a1"},
    {"gitHubDark", "GitHub Dark", "GitHub", true, "#090d13", "#0d1117", "#30363d", "#c9d1d9", "#8b949e", "#58a6ff",
     "#1f6feb", "#39c5cf", "#bc8cff", "#3fb950", "#484f58"},
    {"gitHubDarkDimmed", "GitHub Dark Dimmed", "GitHub", true, "#1c2128", "#22272e", "#444c56", "#adbac7", "#768390",
     "#539bf5", "#316dca", "#539bf5", "#ad6ff7", "#57ab5a", "#545d68"},
    {"gitHubLight", "GitHub Light", "GitHub", false, "#f6f8fa", "#ffffff", "#d0d7de", "#24292f", "#57606a", "#0969da",
     "#0969da", "#0550ae", "#8250df", "#1a7f37", "#8c959f"},
    {"gitHubLightHighContrast", "GitHub Light High Contrast", "GitHub", false, "#ffffff", "#ffffff", "#24292f",
     "#010409", "#24292f", "#0550ae", "#0969da", "#0550ae", "#6639ba", "#116329", "#57606a"},
    {"monokaiClassic", "Monokai Classic", "Monokai", true, "#1e1f1c", "#272822", "#3e3d32", "#f8f8f2", "#75715e",
     "#66d9ef", "#66d9ef", "#66d9ef", "#ae81ff", "#a6e22e", "#75715e"},
    {"monokaiPro", "Monokai Pro", "Monokai", true, "#221f22", "#2d2a2e", "#403e41", "#fcfcfa", "#727072", "#78dce8",
     "#ab9df2", "#78dce8", "#ff6188", "#a9dc76", "#727072"},
    {"monokaiCharcoal", "Monokai Charcoal", "Monokai", true, "#19181a", "#222124", "#333136", "#e3e3e1", "#636164",
     "#62c4d4", "#9788d9", "#62c4d4", "#ea5077", "#92c462", "#636164"},
    {"monokaiLight", "Monokai Light", "Monokai", false, "#f7f7f7", "#ffffff", "#e0e0e0", "#272822", "#75715e",
     "#00897b", "#0d47a1", "#00897b", "#8e24aa", "#2e7d32", "#75715e"},
    {"rosePineMain", "Rosé Pine Main", "Rosé Pine", true, "#14121d", "#191724", "#26233a", "#e0def4", "#908caa",
     "#9ccfd8", "#31748f", "#9ccfd8", "#c4a7e7", "#ebbcba", "#6e6a86"},
    {"rosePineMoon", "Rosé Pine Moon", "Rosé Pine", true, "#1c1a27", "#232136", "#393552", "#e0def4", "#908caa",
     "#9ccfd8", "#3e8fb0", "#9ccfd8", "#c4a7e7", "#ea9a97", "#6e6a86"},
    {"rosePineDawn", "Rosé Pine Dawn", "Rosé Pine", false, "#f2e9de", "#faf4ed", "#cecacd", "#575279", "#797593",
     "#56949f", "#286983", "#56949f", "#907aa9", "#d7827e", "#9893a5"},
    {"ayuDark", "Ayu Dark", "Ayu", true, "#0b0e14", "#0f1419", "#1e2530", "#e6e1cf", "#828c99", "#36a3d9", "#39bae6",
     "#95e6cb", "#d4bfff", "#b8cc52", "#4b5666"},
    {"ayuMirage", "Ayu Mirage", "Ayu", true, "#171b24", "#1f2430", "#2e3646", "#cbccc6", "#707a8c", "#5ccfe6",
     "#73d0ff", "#95e6cb", "#d4bfff", "#bae67e", "#5c6773"},
    {"ayuLight", "Ayu Light", "Ayu", false, "#f3f4f5", "#fafafa", "#e1e3e5", "#575f66", "#8a9199", "#36a3d9", "#39bae6",
     "#4cbf99", "#a37acc", "#86b300", "#abb0b6"},
    {"kanagawaWave", "Kanagawa Wave", "Kanagawa", true, "#16161d", "#1f1f28", "#2a2a37", "#dcd7ba", "#727169",
     "#7aa89f", "#7e9cd8", "#6a9589", "#957fb8", "#98bb6c", "#54546d"},
    {"kanagawaDragon", "Kanagawa Dragon", "Kanagawa", true, "#121216", "#181616", "#282727", "#c5c9c5", "#8a8682",
     "#658594", "#8ba4b0", "#658594", "#8992a7", "#8a9a7b", "#504d49"},
    {"kanagawaLotus", "Kanagawa Lotus", "Kanagawa", false, "#ece7dc", "#f2ecbc", "#d5cea3", "#545464", "#716e61",
     "#597b75", "#4d699b", "#597b75", "#766b90", "#6f894e", "#8a8575"},
    {"everforestDarkHard", "Everforest Dark Hard", "Everforest", true, "#272e33", "#2d353b", "#3d484d", "#d3c6aa",
     "#859289", "#83c092", "#7fbbb3", "#7fbbb3", "#d699b6", "#a7c080", "#5c6a72"},
    {"everforestDarkMedium", "Everforest Dark Medium", "Everforest", true, "#2d353b", "#343f44", "#475258", "#d3c6aa",
     "#9da9a0", "#83c092", "#7fbbb3", "#7fbbb3", "#d699b6", "#a7c080", "#5c6a72"},
    {"everforestLight", "Everforest Light", "Everforest", false, "#edf0e5", "#f8f5e4", "#d3c6aa", "#5c6a72", "#829181",
     "#3a94c5", "#3a94c5", "#35a77c", "#df69ba", "#8da101", "#939f91"},
    {"nightOwlDark", "Night Owl Dark", "Night Owl", true, "#01111d", "#011627", "#0b2942", "#d6deeb", "#5f7e97",
     "#7fdbca", "#82aaff", "#21c7a8", "#c792ea", "#22da6e", "#5f7e97"},
    {"lightOwl", "Light Owl", "Night Owl", false, "#eef2f7", "#f0f4f8", "#d9e2ec", "#403f53", "#7a819b", "#0c969b",
     "#288ed7", "#0c969b", "#7f52ca", "#08916a", "#94a3b8"},
    {"materialPalenight", "Material Palenight", "Material", true, "#202330", "#292d3e", "#3b415b", "#bfc7d5", "#676e95",
     "#89ddff", "#82aaff", "#89ddff", "#c792ea", "#c3e88d", "#676e95"},
    {"materialDeepOcean", "Material Deep Ocean", "Material", true, "#090b10", "#0f111a", "#1a1c25", "#cdd2e0",
     "#8a95b0", "#80cbc4", "#82aaff", "#80cbc4", "#c792ea", "#c3e88d", "#4b526d"},
    {"materialLighter", "Material Lighter", "Material", false, "#eceff1", "#fafafa", "#cfd8dc", "#546e7a", "#90a4ae",
     "#39adb5", "#6182b8", "#39adb5", "#7c4dff", "#91b859", "#b0bec5"},
    {"synthWave84Glow", "SynthWave '84 Glow", "SynthWave '84", true, "#201c2d", "#262335", "#3b334e", "#f92aad",
     "#848bbd", "#36f9f6", "#72f1b8", "#36f9f6", "#fe4450", "#72f1b8", "#614d85"},
    {"synthWave84Classic", "SynthWave '84 Classic", "SynthWave '84", true, "#241b2f", "#2a2139", "#463465", "#ffffff",
     "#8a889d", "#36f9f6", "#fe4450", "#36f9f6", "#ff7edb", "#72f1b8", "#614d85"},
    {"cyberpunk2077", "Cyberpunk 2077", "Cyberpunk", true, "#000814", "#000b1e", "#0c2240", "#00ff9f", "#00b8ff",
     "#00f0ff", "#00b8ff", "#00f0ff", "#ff0055", "#00ff9f", "#0c4078"},
    {"cyberpunkScarlet", "Cyberpunk Scarlet", "Cyberpunk", true, "#120914", "#1a0f1d", "#36163b", "#fcee0a", "#ea448f",
     "#00f0ff", "#ff0055", "#fcee0a", "#ff0055", "#00ff9f", "#6b2275"},
    {"shadesOfPurpleSuperDark", "Shades of Purple Super Dark", "Shades of Purple", true, "#1e1e38", "#222244",
     "#38386a", "#e3dfff", "#a599e9", "#00e8c6", "#b362ff", "#00e8c6", "#ff628c", "#fad000", "#575796"},
    {"shadesOfPurpleClassic", "Shades of Purple Classic", "Shades of Purple", true, "#222144", "#2d2b55", "#48447d",
     "#ffffff", "#a599e9", "#79c0ff", "#b362ff", "#79c0ff", "#ff628c", "#fad000", "#5d579e"},
    {"shadesOfPurpleLight", "Shades of Purple Light", "Shades of Purple", false, "#f6f0ff", "#ffffff", "#e5dbf7",
     "#2d1354", "#6c5ce7", "#00b894", "#4d21fc", "#0984e3", "#6c5ce7", "#00b894", "#a29bfe"},
    {"poimandresDark", "Poimandres Dark", "Poimandres", true, "#171922", "#1b1e28", "#272b38", "#e4f0fb", "#767c9d",
     "#5de4c7", "#89ddff", "#add7ff", "#f087bd", "#5de4c7", "#505775"},
    {"poimandresStorm", "Poimandres Storm", "Poimandres", true, "#14171f", "#1a1d26", "#292d3b", "#dbe6f0", "#6b718f",
     "#5de4c7", "#89ddff", "#89ddff", "#e07aa9", "#5de4c7", "#49506b"},
    {"poimandresLight", "Poimandres Light", "Poimandres", false, "#f4f6f8", "#ffffff", "#e1e4e8", "#1b1e28", "#506477",
     "#2188ff", "#3e6ee8", "#1f6feb", "#8957e5", "#2ea043", "#506477"},
    {"horizonDark", "Horizon Dark", "Horizon", true, "#16171f", "#1c1e26", "#2a2c3a", "#e3e6ee", "#6c6f93", "#26bbd9",
     "#25b0bc", "#26bbd9", "#ee64ac", "#29d398", "#4f5270"},
    {"horizonBright", "Horizon Bright", "Horizon", false, "#ebebed", "#f5f5f7", "#d2d3d9", "#323443", "#787b91",
     "#1d899e", "#1d899e", "#1d899e", "#b83a78", "#1d9c6e", "#9ea1b5"},
    {"andromedaDark", "Andromeda Dark", "Andromeda", true, "#21242c", "#262a33", "#383e4c", "#e1e3e8", "#747d91",
     "#00e8c6", "#00d4ff", "#00e8c6", "#c74ded", "#98e024", "#565e70"},
    {"andromedaBordered", "Andromeda Bordered", "Andromeda", true, "#1b1e24", "#21252d", "#3c4352", "#d7d9df",
     "#6a7387", "#00e8c6", "#00d4ff", "#00e8c6", "#ff00aa", "#98e024", "#545b6e"},
    {"andromedaLight", "Andromeda Light", "Andromeda", false, "#f7f7f8", "#ffffff", "#e0e1e6", "#1e2029", "#747785",
     "#0099b8", "#1e78ff", "#0099b8", "#8a44c8", "#00a854", "#747785"},
    {"nightfoxDark", "Nightfox Dark", "Nightfox", true, "#131a24", "#192330", "#29394f", "#cdcecf", "#71839b",
     "#719cd6", "#86abdc", "#63cdcf", "#9d79d6", "#81b29a", "#495d7a"},
    {"duskfox", "Duskfox", "Nightfox", true, "#191726", "#232136", "#373354", "#cdcbe0", "#7a769a", "#a3be8c",
     "#86abdc", "#eb998b", "#b48ead", "#a3be8c", "#524d73"},
    {"dawnfox", "Dawnfox", "Nightfox", false, "#ebe5df", "#f6f2ee", "#ded5cd", "#575279", "#7d758f", "#286983",
     "#286983", "#286983", "#89749e", "#618774", "#9890a8"},
    {"cobalt2Classic", "Cobalt2 Classic", "Cobalt2", true, "#15232d", "#193549", "#244b67", "#ffffff", "#7ea1b8",
     "#0088ff", "#0088ff", "#0088ff", "#ff9d00", "#3cd070", "#49718d"},
    {"cobalt2Bright", "Cobalt2 Bright", "Cobalt2", true, "#111d26", "#152c3d", "#1e3e56", "#e6f1f8", "#6f91a8",
     "#1fa0ff", "#ffc600", "#1fa0ff", "#ff628c", "#3cd070", "#456a85"},
    {"cobalt2Light", "Cobalt2 Light", "Cobalt2", false, "#edf3fa", "#ffffff", "#d0e1f5", "#193549", "#4f7899",
     "#0088cc", "#0066cc", "#0088cc", "#5c39b5", "#2b8a3e", "#4f7899"},
    {"alabasterDark", "Alabaster Dark", "Alabaster", true, "#0a0e12", "#0e1419", "#21272e", "#f0f2f4", "#6f7a85",
     "#4eb0e6", "#4eb0e6", "#4eb0e6", "#cf7ad0", "#60b050", "#4a535e"},
    {"alabasterLight", "Alabaster Light", "Alabaster", false, "#ededed", "#f7f7f7", "#d8d8d8", "#202020", "#707070",
     "#325cc0", "#325cc0", "#325cc0", "#7a3e9d", "#438234", "#999999"},
    {"tomorrowNight", "Tomorrow Night", "Tomorrow", true, "#151718", "#1d1f21", "#282a2e", "#c5c8c6", "#969896",
     "#8abeb7", "#81a2be", "#8abeb7", "#b294bb", "#b5bd68", "#5c5f63"},
    {"tomorrowNightBlue", "Tomorrow Night Blue", "Tomorrow", true, "#001b3d", "#002451", "#00346e", "#ffffff",
     "#7285b7", "#99ffff", "#bbdaff", "#99ffff", "#ebbbff", "#d1f1a9", "#495a85"},
    {"tomorrowNightEighties", "Tomorrow Night Eighties", "Tomorrow", true, "#232323", "#2d2d2d", "#393939", "#cccccc",
     "#999999", "#66cccc", "#6699cc", "#66cccc", "#cc99cc", "#99cc99", "#666666"},
    {"tomorrowNightBright", "Tomorrow Night Bright", "Tomorrow", true, "#000000", "#101010", "#2a2a2a", "#eaeaea",
     "#969896", "#70c0b1", "#7aa6da", "#70c0b1", "#c397d8", "#b9ca4a", "#555555"},
    {"tomorrowDay", "Tomorrow Day", "Tomorrow", false, "#f5f5f5", "#ffffff", "#d6d6d6", "#4d4d4c", "#8e908c", "#3e999f",
     "#4271ae", "#3e999f", "#8959a8", "#718c00", "#a0a2a0"},
};

inline constexpr size_t kTotalThemePaletteCount = sizeof(kAllThemePalettes) / sizeof(kAllThemePalettes[0]);

inline const ThemePalette *findThemePaletteById(const QString &id) {
    if (id.isEmpty()) {
        return nullptr;
    }
    if (id.compare(QLatin1String("defaultLight"), Qt::CaseInsensitive) == 0) {
        return findThemePaletteById(QStringLiteral("notepadNext"));
    }
    if (id.compare(QLatin1String("defaultDark"), Qt::CaseInsensitive) == 0) {
        return findThemePaletteById(QStringLiteral("dracula"));
    }
    for (size_t i = 0; i < kTotalThemePaletteCount; ++i) {
        if (id.compare(QLatin1String(kAllThemePalettes[i].id), Qt::CaseInsensitive) == 0 ||
            id.compare(QLatin1String(kAllThemePalettes[i].displayName), Qt::CaseInsensitive) == 0) {
            return &kAllThemePalettes[i];
        }
    }
    return nullptr;
}

inline QStringList getThemeFamiliesList() {
    QStringList families;
    for (size_t i = 0; i < kTotalThemePaletteCount; ++i) {
        QString f = QLatin1String(kAllThemePalettes[i].family);
        if (!families.contains(f)) {
            families << f;
        }
    }
    return families;
}

inline QList<const ThemePalette *> getThemePalettesByFamily(const QString &family) {
    QList<const ThemePalette *> list;
    for (size_t i = 0; i < kTotalThemePaletteCount; ++i) {
        if (family.compare(QLatin1String(kAllThemePalettes[i].family), Qt::CaseInsensitive) == 0) {
            list.append(&kAllThemePalettes[i]);
        }
    }
    return list;
}

inline QList<const ThemePalette *> getThemesByDarkMode(bool isDark) {
    QList<const ThemePalette *> list;
    for (size_t i = 0; i < kTotalThemePaletteCount; ++i) {
        if (kAllThemePalettes[i].isDark == isDark) {
            list.append(&kAllThemePalettes[i]);
        }
    }
    return list;
}

#endif  // THEMEPALETTEDATA_H
