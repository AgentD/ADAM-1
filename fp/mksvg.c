#include <stdio.h>
#include <string.h>

#define WIDTH 297
#define HEIGHT 420

#define LED_HOLE 8
#define LED_DIST 10

#define SW_HOLE 6
#define SW_RING 12
#define SW_RING_HOLE 2
#define SW_DIST_H 17
#define SW_DIST_V 15

#define BAR_WIDTH "1.5"

#define BUTT_HOLE 16
#define BUTT_DIST_H 27
#define BUTT_DIST_V 20

#define TEXT_HEIGHT 6
#define TEXT_DIST_V 13

const char *svg_hdr =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
"\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
"<svg xmlns=\"http://www.w3.org/2000/svg\"\n"
"     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
"     xmlns:ev=\"http://www.w3.org/2001/xml-events\"\n"
"     version=\"1.1\" baseProfile=\"full\"\n"
"     width=\"%dmm\" height=\"%dmm\">\n";

const char *circle_fmt =
"<circle cx=\"%dmm\" cy=\"%dmm\" r=\"%dmm\" "
"stroke=\"none\" fill=\"black\" />\n";

const char *line_fmt =
"<line x1=\"%dmm\" y1=\"%dmm\" x2=\"%dmm\" y2=\"%dmm\" "
"stroke=\"black\" stroke-width=\"%smm\" stroke-linecap=\"square\"/>\n";

const char *text_fmt =
"<text x=\"%dmm\" y=\"%dmm\" fill=\"black\" stroke=\"none\" "
"font-size=\"%dmm\" "
"font-family=\"./OpenSans-Regular.ttf\" text-anchor=\"middle\">";

const char *text_end = "</text>\n";

static void make_text_bar(int from_top, int x0, int x1, const char *heading)
{
	int i, x, hlen = TEXT_HEIGHT;
	const char *ptr;

	/* UGH */
	for (ptr = heading; *ptr; ++ptr) {
		if (strchr("QOCEUMW", *ptr))
			hlen += (3 * TEXT_HEIGHT) / 4;
		else if (strchr("IJ", *ptr))
			hlen += TEXT_HEIGHT / 4;
		else
			hlen += TEXT_HEIGHT / 2;
	}

	printf(line_fmt, x0, from_top, x0/2 + x1/2 - hlen/2,
		from_top, BAR_WIDTH);
	printf(line_fmt, x0/2 + x1/2 + hlen/2, from_top, x1,
		from_top, BAR_WIDTH);

	printf(line_fmt, x0, from_top, x0, from_top + LED_DIST, BAR_WIDTH);
	printf(line_fmt, x1, from_top, x1, from_top + LED_DIST, BAR_WIDTH);

	printf(text_fmt, x0/2 + x1/2, from_top + TEXT_HEIGHT/3, TEXT_HEIGHT);
	printf("%s%s", heading, text_end);
}

static void make_label_row(int from_top, int from_left, int spacing,
			int count, const char **labels)
{
	int i, x;

	for (i = 0, x = from_left; i < count; ++i, x += spacing) {
		printf(text_fmt, x, from_top, TEXT_HEIGHT);

		if (labels)
			printf("%s%s", labels[i], text_end);
		else
			printf("%d%s", count - 1 - i, text_end);
	}
}

static void make_led_row(int from_top, int from_left, int spacing,
			int count, const char *heading,
			const char **labels)
{
	int i, x, x0, x1;

	x0 = from_left - LED_HOLE;
	x1 = from_left + (count - 1) * spacing + LED_HOLE;

	make_text_bar(from_top, x0, x1, heading);

	from_top += LED_DIST;

	for (i = 0, x = from_left; i < count; ++i, x += spacing)
		printf(circle_fmt, x, from_top, LED_HOLE / 2);

	from_top += TEXT_DIST_V;
	make_label_row(from_top, from_left, spacing, count, labels);
}

static void make_switch_row(int from_top, int from_left,
				int count, const char *heading,
				const char **labels)
{
	int i, x;

	make_led_row(from_top, from_left, SW_DIST_H, count, heading, labels);

	from_top += LED_DIST + LED_HOLE + SW_DIST_V;

	for (i = 0, x = from_left; i < count; ++i, x += SW_DIST_H) {
		printf(circle_fmt, x, from_top, SW_HOLE / 2);

		printf(circle_fmt, x, from_top + SW_RING / 2,
			SW_RING_HOLE / 2);
	}
}

static void make_button_row(int from_top, int from_left,
				int count, const char *heading,
				const char **labels)
{
	int i, x;

	make_led_row(from_top, from_left, BUTT_DIST_H, count, heading, labels);

	from_top += LED_DIST + LED_HOLE + BUTT_DIST_V;

	for (i = 0, x = from_left; i < count; ++i, x += BUTT_DIST_H)
		printf(circle_fmt, x, from_top, BUTT_HOLE / 2);
}

const char *fctl_labels[] = { "AINV", "+C", "-C", "-U", "-M", "PSEL" };

const char *ctrl_labels[] = {
	"AWR", "BWR", "REGW", "WC", "MEMW",
	"PSWR", "TPW", "CLU", "ULF", "UCF"
};

const char *psw_labels[] = { "M", "U", "I", "S", "Z", "C" };
const char *stat_labels[] = { "INT", "PF", "M1", "IWR", "FIN" };

const char *i_lbl[] = { "15" };
const char *op_lbl[] = { "14", "13", "12", "11", "10", "9" };
const char *m_lbl[] = { "8", "7", "6" };
const char *s_lbl[] = { "5", "4", "3" };
const char *d_lbl[] = { "2", "1", "0" };

int main(void)
{
	int y = 10;

	printf(svg_hdr, WIDTH, HEIGHT);

	make_text_bar(y, 21 - LED_HOLE,
			21 + 15 * SW_DIST_H + LED_HOLE, "INST");

	y += 8;
	make_led_row(y, 21,                SW_DIST_H, 1, "I", i_lbl);
	make_led_row(y, 21 + SW_DIST_H,    SW_DIST_H, 6, "OPCODE", op_lbl);
	make_led_row(y, 21 + SW_DIST_H*7,  SW_DIST_H, 3, "M", m_lbl);
	make_led_row(y, 21 + SW_DIST_H*10, SW_DIST_H, 3, "SRC", s_lbl);
	make_led_row(y, 21 + SW_DIST_H*13, SW_DIST_H, 3, "DST", d_lbl);

	y += 29;
	make_led_row(y, 21 + 10*SW_DIST_H, SW_DIST_H,
			sizeof(psw_labels)/sizeof(psw_labels[0]),
			"PSW", psw_labels);

	make_led_row(y, 21, SW_DIST_H,
			sizeof(stat_labels)/sizeof(stat_labels[0]),
			"STAT", stat_labels);

	y += 29;
	make_led_row(y, 21, SW_DIST_H, 16, "A", NULL);

	y += 29;
	make_led_row(y, 21, SW_DIST_H, 16, "B", NULL);

	y += 29;
	make_switch_row(y, 21, 16, "DATA", NULL);

	y += 45;
	make_switch_row(y, 21, 5, "SRC", NULL);
	make_switch_row(y, 21 + 6*SW_DIST_H, 3, "DST", NULL);
	make_switch_row(y, 21 + 10*SW_DIST_H,
			sizeof(fctl_labels)/sizeof(fctl_labels[0]),
			"FCTRL", fctl_labels);
	y += 45;
	make_button_row(y, 27,
			sizeof(ctrl_labels)/sizeof(ctrl_labels[0]), "CTRL",
			ctrl_labels);

	puts("</svg>");
	return 0;
}

