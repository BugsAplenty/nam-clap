// NAMPluginUI.cpp — DPF Cairo UI for NAM (portable: modern DGL::Cairo or weak legacy hook)
// - Text field for .nam file path
// - "Load" button -> setState("modelPath", path)
// - Primary path: DGL::Cairo::GraphicsContext (newer DPF)
// - Fallback: weak extern dgl_cairo_get_context (older DPF); links even if missing

#include "DistrhoUI.hpp"
#include <algorithm>
#include <cctype>
#include <string>

#if __has_include("dgl/Cairo.hpp")
  #define NAM_HAS_DGL_CAIRO 1
  #include "dgl/Cairo.hpp"
  #include <cairo/cairo.h>
#else
  #include <cairo/cairo.h>
  // Declare legacy hook as a weak symbol so link succeeds when it doesn't exist.
  extern "C" cairo_t* dgl_cairo_get_context(DGL::Widget* widget) __attribute__((weak));
#endif

START_NAMESPACE_DISTRHO

static inline bool pointIn(float x, float y, float X, float Y, float W, float H) noexcept
{
    return (x >= X && x <= X+W && y >= Y && y <= Y+H);
}

class NAMPluginUI final : public UI
{
public:
    NAMPluginUI()
        : UI(680, 180) { setGeometryConstraints(420, 120, false); }

protected:
    void onDisplay() override
    {
        cairo_t* cr = nullptr;

#if NAM_HAS_DGL_CAIRO
        // Newer DPF path
        {
            DGL::Cairo::GraphicsContext gc(*this);
            cr = gc.get();
            if (cr == nullptr) return;
            draw(cr);
        }
#else
        // Legacy hook path, may be null if symbol doesn't exist in this DPF
        if (dgl_cairo_get_context != nullptr) {
            cr = dgl_cairo_get_context(this);
            if (cr == nullptr) return;
            draw(cr);
        } else {
            // No usable Cairo context available in this DPF build.
            // Draw nothing to avoid crashes; you still can load models via host state restore.
        }
#endif
    }

    bool onMouse(const MouseEvent& ev) override
    {
        mouseX_ = ev.pos.getX();
        mouseY_ = ev.pos.getY();

        const float fieldX = 16.f, fieldY = 68.f;
        const float fieldW = std::max(280.f, getWidth() - 160.f);
        const float fieldH = 34.f;
        const float btnW = 96.f,  btnH = fieldH;
        const float btnX = fieldX + fieldW + 14.f, btnY = fieldY;

        if (ev.press && ev.button == 1) {
            if (pointIn(mouseX_, mouseY_, fieldX, fieldY, fieldW, fieldH)) {
                focused_ = true; repaint(); return true;
            }
            if (pointIn(mouseX_, mouseY_, btnX, btnY, btnW, btnH)) {
                sendPathToDSP(); repaint(); return true;
            }
            focused_ = false; repaint();
        }
        return false;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        mouseX_ = ev.pos.getX();
        mouseY_ = ev.pos.getY();
        repaint();
        return false;
    }

    // Older DPF: only .key/.mod are reliable
    bool onKeyboard(const KeyboardEvent& ev) override
    {
        if (!focused_ || !ev.press) return false;

        if (ev.key == kKeyBackspace) {
            if (!modelPath_.empty()) { modelPath_.pop_back(); repaint(); }
            return true;
        }
        if (ev.key == kKeyEnter) { sendPathToDSP(); return true; }

        if ((ev.mod & (kModifierControl | kModifierAlt)) == 0) {
            const uint32_t key = ev.key;
            if (key >= 32 && key < 127) {
                const char ch = static_cast<char>(key);
                if (std::isalnum(static_cast<unsigned char>(ch)) ||
                    ch=='/' || ch=='.' || ch=='_' || ch=='-' ||
    #ifdef _WIN32
                    ch=='\\' || ch==':' ||
    #endif
                    ch==' ') {
                    modelPath_.push_back(ch);
                    repaint();
                    return true;
                }
            }
        }
        return false;
    }

private:
    void draw(cairo_t* cr)
    {
        const float W = getWidth();
        const float H = getHeight();

        // Background
        cairo_save(cr);
        cairo_rectangle(cr, 0, 0, W, H);
        cairo_set_source_rgb(cr, 0.20, 0.22, 0.26);
        cairo_fill(cr);
        cairo_restore(cr);

        // Title
        cairo_save(cr);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 18.0);
        cairo_set_source_rgb(cr, 0.96, 0.97, 0.99);
        cairo_move_to(cr, 16, 24);
        cairo_show_text(cr, "Neural Amp Modeler — Load .nam model");
        cairo_restore(cr);

        // Label
        cairo_save(cr);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 13.0);
        cairo_set_source_rgb(cr, 0.90, 0.92, 0.95);
        cairo_move_to(cr, 16, 56);
        cairo_show_text(cr, "Model path:");
        cairo_restore(cr);

        // Field geometry
        const float fieldX = 16.0f;
        const float fieldY = 68.0f;
        const float fieldW = std::max(280.0f, W - 160.0f);
        const float fieldH = 34.0f;

        // Field background + border
        cairo_save(cr);
        roundedRect(cr, fieldX, fieldY, fieldW, fieldH, 6.0f);
        const bool hover = pointIn(mouseX_, mouseY_, fieldX, fieldY, fieldW, fieldH);
        const double fR = focused_ ? 0.24 : (hover ? 0.22 : 0.21);
        const double fG = focused_ ? 0.26 : (hover ? 0.24 : 0.23);
        const double fB = focused_ ? 0.30 : (hover ? 0.28 : 0.27);
        cairo_set_source_rgb(cr, fR, fG, fB);
        cairo_fill_preserve(cr);
        cairo_set_line_width(cr, 1.2);
        cairo_set_source_rgb(cr, focused_ ? 0.45 : 0.35,
                                  focused_ ? 0.65 : 0.35,
                                  focused_ ? 0.98 : 0.40);
        cairo_stroke(cr);
        cairo_restore(cr);

        // Text (clipped inside field)
        cairo_save(cr);
        cairo_rectangle(cr, fieldX+8, fieldY+6, fieldW-16, fieldH-12);
        cairo_clip(cr);

        cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 14.0);
        if (modelPath_.empty() && !focused_) {
            cairo_set_source_rgb(cr, 0.75, 0.78, 0.82);
            cairo_move_to(cr, fieldX+12, fieldY + fieldH*0.65f);
            cairo_show_text(cr, "/full/path/to/model.nam");
        } else {
            cairo_set_source_rgb(cr, 0.98, 0.99, 1.00);
            cairo_move_to(cr, fieldX+12, fieldY + fieldH*0.65f);
            cairo_show_text(cr, modelPath_.c_str());
        }

        // Caret at end (static)
        if (focused_) {
            cairo_text_extents_t te{};
            cairo_text_extents(cr, modelPath_.c_str(), &te);
            double cx = fieldX + 12 + te.width;
            if (cx > fieldX + fieldW - 12) cx = fieldX + fieldW - 12;
            cairo_set_source_rgb(cr, 0.90, 0.95, 1.0);
            cairo_set_line_width(cr, 1.0);
            cairo_move_to(cr, cx, fieldY + 6);
            cairo_line_to(cr, cx, fieldY + fieldH - 6);
            cairo_stroke(cr);
        }
        cairo_restore(cr);

        // Load button
        const float btnW = 96.0f;
        const float btnH = fieldH;
        const float btnX = fieldX + fieldW + 14.0f;
        const float btnY = fieldY;

        const bool hovBtn = pointIn(mouseX_, mouseY_, btnX, btnY, btnW, btnH);
        cairo_save(cr);
        roundedRect(cr, btnX, btnY, btnW, btnH, 6.0f);
        cairo_set_source_rgb(cr, hovBtn ? 0.30 : 0.24,
                                  hovBtn ? 0.70 : 0.55,
                                  hovBtn ? 0.95 : 0.85);
        cairo_fill(cr);

        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 14.0);
        cairo_set_source_rgb(cr, 0.08, 0.10, 0.12);
        centerText(cr, "Load", btnX, btnY, btnW, btnH);
        cairo_restore(cr);

        // Hint
        cairo_save(cr);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 12.0);
        cairo_set_source_rgb(cr, 0.88, 0.90, 0.94);
        cairo_move_to(cr, 16, fieldY + fieldH + 26);
        cairo_show_text(cr, "Type a full path and press Enter or click Load.");
        cairo_restore(cr);
    }

    void sendPathToDSP()
    {
        auto trim = [](std::string& s){
            auto issp = [](unsigned char c){ return std::isspace(c) != 0; };
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](char c){ return !issp((unsigned char)c); }));
            s.erase(std::find_if(s.rbegin(), s.rend(), [&](char c){ return !issp((unsigned char)c); }).base(), s.end());
        };

        std::string path = modelPath_;
        trim(path);
        if (path.empty()) return;

        setState("modelPath", path.c_str()); // Plugin::setState must load it
        repaint();
    }

    static void roundedRect(cairo_t* cr, float x, float y, float w, float h, float r)
    {
        const float x2 = x + w, y2 = y + h;
        cairo_new_sub_path(cr);
        cairo_arc(cr, x+r,  y+r,  r, M_PI,       1.5*M_PI);
        cairo_arc(cr, x2-r, y+r,  r, 1.5*M_PI,   0);
        cairo_arc(cr, x2-r, y2-r, r, 0,          0.5*M_PI);
        cairo_arc(cr, x+r,  y2-r, r, 0.5*M_PI,   M_PI);
        cairo_close_path(cr);
    }

    static void centerText(cairo_t* cr, const char* txt, float x, float y, float w, float h)
    {
        cairo_text_extents_t te{};
        cairo_text_extents(cr, txt, &te);
        const float tx = x + (w - te.width)/2 - te.x_bearing;
        const float ty = y + (h - te.height)/2 - te.y_bearing;
        cairo_move_to(cr, tx, ty);
        cairo_show_text(cr, txt);
    }

private:
    std::string modelPath_;
    bool  focused_ = false;
    float mouseX_ = 0.f;
    float mouseY_ = 0.f;

    DISTRHO_DECLARE_NON_COPYABLE(NAMPluginUI)
};

UI* createUI() { return new NAMPluginUI(); }

END_NAMESPACE_DISTRHO
