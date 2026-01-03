// Coded by emarti, Murat Özdemir
// (C) 2026 Jan, Under the MIT License
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include <string>
#include <algorithm>
#include <cstdio>
#include <filesystem>

namespace fs = std::filesystem;

/* Check for .sid extension */
static bool is_sid(const std::string& path) {
    if (path.size() < 4) return false;
    std::string ext = path.substr(path.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".sid";
}

/* aa.sid -> aa_reloc.sid */
static std::string make_reloc_name(const std::string& sid_path) {
    fs::path p(sid_path);
    fs::path out =
        p.parent_path() /
        (p.stem().string() + "_reloc" + p.extension().string());
    return out.string();
}

/* Run sidreloc (from PATH, with 2 arguments) */
static std::string run_sidreloc(const std::string& in_sid,
                                const std::string& out_sid) {
    std::string cmd =
        "sidreloc \"" + in_sid + "\" \"" + out_sid + "\" 2>&1";

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe)
        return "ERROR: Unable to run sidreloc (check PATH)";

    char buffer[512];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    _pclose(pipe);

    if (result.empty())
        result = "(No output)";

    result += "\nGenerated file:\n" + out_sid;
    return result;
}

class DropBox : public Fl_Box {
    Fl_Text_Buffer* textbuf;

public:
    DropBox(int X, int Y, int W, int H, Fl_Text_Buffer* buf)
        : Fl_Box(X, Y, W, H, "Drop a .sid file here"),
          textbuf(buf) {

        box(FL_FLAT_BOX);
        color(FL_DARK3);
        labelcolor(FL_WHITE);
        align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    }

    int handle(int e) override {
        switch (e) {
        case FL_DND_ENTER:
        case FL_DND_DRAG:
        case FL_DND_RELEASE:
            return 1;

        case FL_PASTE: {
            const char* txt = Fl::event_text();
            if (!txt) return 1;

            std::string all(txt);
            std::string in_path = all.substr(0, all.find('\n'));

            if (!is_sid(in_path)) {
                label("Only .sid files are accepted");
                redraw();
                return 1;
            }

            std::string out_path = make_reloc_name(in_path);

            label(in_path.c_str());
            redraw();

            textbuf->text("Running sidreloc...\n");

            std::string output = run_sidreloc(in_path, out_path);
            textbuf->text(output.c_str());
            return 1;
        }
        }
        return Fl_Box::handle(e);
    }
};

int main() {
    Fl_Window win(720, 420, "SID Reloc GUI");

    Fl_Text_Buffer* buffer = new Fl_Text_Buffer();

    DropBox drop(20, 20, 680, 80, buffer);

    Fl_Text_Display output(20, 120, 680, 270);
    output.buffer(buffer);

    win.end();
    win.show();
    return Fl::run();
}
