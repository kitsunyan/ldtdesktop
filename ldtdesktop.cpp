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

OVERRIDE(font_set_pixel_size, _ZN5QFont12setPixelSizeEi,
	void, ARGS(QFont * font, int pixel_size), {
	bool handled = false;
	if (font->family() == "Open Sans") {
		struct font_replacement_t * fr = get_font_replacement_normal();
		if (fr) {
			font->setFamily(fr->family);
			font->setWeight(QFont::Normal);
			font_set_pixel_size(font, fr->size > 0 ? fr->size : pixel_size);
			font->setKerning(fr->kerning);
			handled = true;
			printf("kerning %d\n", fr->kerning);
		}
	} else if (font->family() == "Open Sans Semibold") {
		struct font_replacement_t * fr = get_font_replacement_normal();
		if (fr) {
			font->setFamily(fr->family);
			font->setWeight(QFont::Bold);
			font_set_pixel_size(font, fr->size > 0 ? fr->size : pixel_size);
			font->setKerning(fr->kerning);
			handled = true;
		}
	} else if (font->family() == "monospace" || font->family() == "Consolas" ||
		font->family() == "Liberation Mono" || font->family() == "Menlo" ||
		font->family() == "Courier") {
		struct font_replacement_t * fr = get_font_replacement_monospace();
		if (fr) {
			font->setFamily(fr->family);
			font->setWeight(QFont::Normal);
			font_set_pixel_size(font, fr->size > 0 ? fr->size : pixel_size);
			font->setKerning(fr->kerning);
			handled = true;
		}
	}
	if (!handled) {
		font_set_pixel_size(font, pixel_size);
	}
})