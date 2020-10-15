#include <dlfcn.h>
#include <stdlib.h>

#include <QtCore/QSet>
#include <QtGui/QPainter>

#define ARGS(...) __VA_ARGS__
#define OVERRIDE(name, local, result, args, body) \
typedef result (* name##_t)(args); \
static name##_t name = nullptr; \
extern "C" result local(args) { \
	if (!name) { \
		name = reinterpret_cast<name##_t>(dlsym(RTLD_NEXT, #local)); \
		if (!name) { \
			abort(); \
		} \
	} \
	body \
}

static void update_ld_preload() {
	static bool updated = false;
	if (!updated) {
		updated = true;
		auto list = QString(getenv("LD_PRELOAD")).split(":");
		for (int i = 0; i < list.size(); i++) {
			if (list.at(i).endsWith("/" SONAME) || list.at(i) == SONAME) {
				list.removeAt(i);
				QByteArray lp = list.join(":").toLatin1();
				if (lp.isEmpty()) {
					unsetenv("LD_PRELOAD");
				} else {
					setenv("LD_PRELOAD", lp.data(), 1);
				}
				break;
			}
		}
	}
}

struct font_replacement_t {
	bool ready = false;
	bool enable = false;
	QString family;
	int size;
	bool kerning;
};

static void get_font_replacement(struct font_replacement_t & fr,
	const char * env_font, const char * env_font_size, const char * env_font_kerning) {
	if (!fr.ready) {
		fr.ready = true;
		const char * family = getenv(env_font);
		const char * size_str = getenv(env_font_size);
		int size = atoi(size_str ? size_str : "");
		const char * kerning_str = getenv(env_font_kerning);
		if (family && family[0]) {
			fr.enable = true;
			fr.family = family;
			fr.size = size;
			fr.kerning = !kerning_str || kerning_str[0] &&
				strcmp(kerning_str, "false") && strcmp(kerning_str, "0");
		}
	}
}

static struct font_replacement_t * get_font_replacement_normal() {
	static struct font_replacement_t fr;
	get_font_replacement(fr, "TG_FONT_NORMAL", "TG_FONT_NORMAL_SIZE",
		"TG_FONT_NORMAL_KERNING");
	return fr.enable ? &fr : nullptr;
}

static struct font_replacement_t * get_font_replacement_monospace() {
	static struct font_replacement_t fr;
	get_font_replacement(fr, "TG_FONT_MONOSPACE", "TG_FONT_MONOSPACE_SIZE",
		"TG_FONT_MONOSPACE_KERNING");
	return fr.enable ? &fr : nullptr;
}

static bool apply_font(QFont *, int, const struct font_replacement_t *);

OVERRIDE(font_set_pixel_size, _ZN5QFont12setPixelSizeEi,
	void, ARGS(QFont * font, int pixel_size), {
	update_ld_preload();
	bool handled = false;
	if (font->family() == "Open Sans" ||
		font->family() == "Open Sans Semibold") {
		handled = apply_font(font, pixel_size,
			get_font_replacement_normal());
	} else if (font->family() == "monospace" ||
		font->family() == "Consolas" ||
		font->family() == "Liberation Mono" ||
		font->family() == "Menlo" ||
		font->family() == "Courier") {
		handled = apply_font(font, pixel_size,
			get_font_replacement_monospace());
	}
	if (!handled) {
		font_set_pixel_size(font, pixel_size);
	}
})

static bool apply_font(QFont * font, int pixel_size,
	const struct font_replacement_t * fr) {
	if (fr) {
		font->setFamily(fr->family);
		font_set_pixel_size(font, fr->size > 0 ? fr->size : pixel_size);
		font->setKerning(fr->kerning);
		return true;
	} else {
		return false;
	}
}
