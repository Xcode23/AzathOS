#include <stdint.h>

#include "interrupts.hpp"
#include "tty.hpp"
#include "kernel.hpp"
#include "IO.hpp"

const uint16_t IDT_SIZE = 256;

/**
 * struct for registers saved in stack in assembly
 * used as a padding parameter to access the 
 * relevant data deeper in the stack 
**/
struct Registers { 
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
} __attribute__((packed));

/**
 * struct for registers saved in stack in assembly
 * used as a padding parameter to access the 
 * relevant data deeper in the stack. 
**/
struct InterruptData { //TODO: add parameters for interrupt handlers
	uint32_t error_code; 
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

struct IDTEntry {
    uint16_t segment_offset_1; // segment offset bits 0..15
    uint16_t selector; // a code segment selector in GDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t segment_offset_2; // segment offset bits 16..31
} __attribute__((packed));

struct IDTDescriptor {
	uint16_t size;
	uint32_t address;
} __attribute__((packed));

IDTEntry idt[IDT_SIZE];

IDTDescriptor idt_descriptor = {sizeof(IDTEntry) * IDT_SIZE - 1, reinterpret_cast<uint32_t>(&idt)}; //TODO: Add comments

extern "C" void generic_interrupt_handler(Registers regs, uint32_t intr_num, InterruptData intrd);
void setup_isr(uint8_t intr_num, uintptr_t handler);
void pic_send_eoi(uint8_t irq);
void stub_isr(uint32_t intr_num);
void exception_handler(uint32_t intr_num, uint32_t error_code);
void hardware_irq_handler(uint32_t intr_num);
void timer_irq_handler();
void keyboard_irq_handler();


void generic_interrupt_handler(Registers regs, uint32_t intr_num, InterruptData intrd){
	asm("cli");
	if (intr_num <= 31) { //TODO: improve exception check
		exception_handler(intr_num, intrd.error_code);
	} else if (intr_num == 32) {
		timer_irq_handler();
	} else if (intr_num == 33) {
		keyboard_irq_handler();
	} else if (intr_num >= 34 && intr_num <= 48) {
		hardware_irq_handler(intr_num);
	} else {
		stub_isr(intr_num);
	}
	asm("sti");
}

void stub_isr(uint32_t intr_num) {
	char str[256];
	itoa(intr_num, str);
	terminal_writestring(str);
	terminal_writestring("\n");
}

void exception_handler(uint32_t intr_num, uint32_t error_code) {
	char str[256];
	itoa(intr_num, str);
	terminal_writestring(str);
	if (error_code != 0) {
		char str2[256];
		itoa(error_code, str2);
		terminal_writestring("   ");
		terminal_writestring(str2);
	}
	terminal_writestring("\n");
	panic();
}

void hardware_irq_handler(uint32_t intr_num) {
	char str[256];
	itoa(intr_num, str);
	terminal_writestring(str);
	terminal_writestring("\n");
	pic_send_eoi(intr_num-32);
}

void timer_irq_handler() {
	pic_send_eoi(0);
}

void keyboard_irq_handler() {
	auto scan_code = inb(0x60);
	char str[256];
	itoa(scan_code, str);
	terminal_writestring(str);
	terminal_writestring("\n");
	pic_send_eoi(1);
}

void setup_idt(void) {
	setup_isr(0, reinterpret_cast<uintptr_t>(interrupt_handler_0));
	setup_isr(1, reinterpret_cast<uintptr_t>(interrupt_handler_1));
	setup_isr(2, reinterpret_cast<uintptr_t>(interrupt_handler_2));
	setup_isr(3, reinterpret_cast<uintptr_t>(interrupt_handler_3));
	setup_isr(4, reinterpret_cast<uintptr_t>(interrupt_handler_4));
	setup_isr(5, reinterpret_cast<uintptr_t>(interrupt_handler_5));
	setup_isr(6, reinterpret_cast<uintptr_t>(interrupt_handler_6));
	setup_isr(7, reinterpret_cast<uintptr_t>(interrupt_handler_7));
	setup_isr(8, reinterpret_cast<uintptr_t>(interrupt_handler_8));
	setup_isr(9, reinterpret_cast<uintptr_t>(interrupt_handler_9));
	setup_isr(10, reinterpret_cast<uintptr_t>(interrupt_handler_10));
	setup_isr(11, reinterpret_cast<uintptr_t>(interrupt_handler_11));
	setup_isr(12, reinterpret_cast<uintptr_t>(interrupt_handler_12));
	setup_isr(13, reinterpret_cast<uintptr_t>(interrupt_handler_13));
	setup_isr(14, reinterpret_cast<uintptr_t>(interrupt_handler_14));
	setup_isr(15, reinterpret_cast<uintptr_t>(interrupt_handler_15));
	setup_isr(16, reinterpret_cast<uintptr_t>(interrupt_handler_16));
	setup_isr(17, reinterpret_cast<uintptr_t>(interrupt_handler_17));
	setup_isr(18, reinterpret_cast<uintptr_t>(interrupt_handler_18));
	setup_isr(19, reinterpret_cast<uintptr_t>(interrupt_handler_19));
	setup_isr(20, reinterpret_cast<uintptr_t>(interrupt_handler_20));
	setup_isr(21, reinterpret_cast<uintptr_t>(interrupt_handler_21));
	setup_isr(22, reinterpret_cast<uintptr_t>(interrupt_handler_22));
	setup_isr(23, reinterpret_cast<uintptr_t>(interrupt_handler_23));
	setup_isr(24, reinterpret_cast<uintptr_t>(interrupt_handler_24));
	setup_isr(25, reinterpret_cast<uintptr_t>(interrupt_handler_25));
	setup_isr(26, reinterpret_cast<uintptr_t>(interrupt_handler_26));
	setup_isr(27, reinterpret_cast<uintptr_t>(interrupt_handler_27));
	setup_isr(28, reinterpret_cast<uintptr_t>(interrupt_handler_28));
	setup_isr(29, reinterpret_cast<uintptr_t>(interrupt_handler_29));
	setup_isr(30, reinterpret_cast<uintptr_t>(interrupt_handler_30));
	setup_isr(31, reinterpret_cast<uintptr_t>(interrupt_handler_31));
	setup_isr(32, reinterpret_cast<uintptr_t>(interrupt_handler_32));
	setup_isr(33, reinterpret_cast<uintptr_t>(interrupt_handler_33));
	setup_isr(34, reinterpret_cast<uintptr_t>(interrupt_handler_34));
	setup_isr(35, reinterpret_cast<uintptr_t>(interrupt_handler_35));
	setup_isr(36, reinterpret_cast<uintptr_t>(interrupt_handler_36));
	setup_isr(37, reinterpret_cast<uintptr_t>(interrupt_handler_37));
	setup_isr(38, reinterpret_cast<uintptr_t>(interrupt_handler_38));
	setup_isr(39, reinterpret_cast<uintptr_t>(interrupt_handler_39));
	setup_isr(40, reinterpret_cast<uintptr_t>(interrupt_handler_40));
	setup_isr(41, reinterpret_cast<uintptr_t>(interrupt_handler_41));
	setup_isr(42, reinterpret_cast<uintptr_t>(interrupt_handler_42));
	setup_isr(43, reinterpret_cast<uintptr_t>(interrupt_handler_43));
	setup_isr(44, reinterpret_cast<uintptr_t>(interrupt_handler_44));
	setup_isr(45, reinterpret_cast<uintptr_t>(interrupt_handler_45));
	setup_isr(46, reinterpret_cast<uintptr_t>(interrupt_handler_46));
	setup_isr(47, reinterpret_cast<uintptr_t>(interrupt_handler_47));
	setup_isr(48, reinterpret_cast<uintptr_t>(interrupt_handler_48));
	setup_isr(49, reinterpret_cast<uintptr_t>(interrupt_handler_49));
	setup_isr(50, reinterpret_cast<uintptr_t>(interrupt_handler_50));
	setup_isr(51, reinterpret_cast<uintptr_t>(interrupt_handler_51));
	setup_isr(52, reinterpret_cast<uintptr_t>(interrupt_handler_52));
	setup_isr(53, reinterpret_cast<uintptr_t>(interrupt_handler_53));
	setup_isr(54, reinterpret_cast<uintptr_t>(interrupt_handler_54));
	setup_isr(55, reinterpret_cast<uintptr_t>(interrupt_handler_55));
	setup_isr(56, reinterpret_cast<uintptr_t>(interrupt_handler_56));
	setup_isr(57, reinterpret_cast<uintptr_t>(interrupt_handler_57));
	setup_isr(58, reinterpret_cast<uintptr_t>(interrupt_handler_58));
	setup_isr(59, reinterpret_cast<uintptr_t>(interrupt_handler_59));
	setup_isr(60, reinterpret_cast<uintptr_t>(interrupt_handler_60));
	setup_isr(61, reinterpret_cast<uintptr_t>(interrupt_handler_61));
	setup_isr(62, reinterpret_cast<uintptr_t>(interrupt_handler_62));
	setup_isr(63, reinterpret_cast<uintptr_t>(interrupt_handler_63));
	setup_isr(64, reinterpret_cast<uintptr_t>(interrupt_handler_64));
	setup_isr(65, reinterpret_cast<uintptr_t>(interrupt_handler_65));
	setup_isr(66, reinterpret_cast<uintptr_t>(interrupt_handler_66));
	setup_isr(67, reinterpret_cast<uintptr_t>(interrupt_handler_67));
	setup_isr(68, reinterpret_cast<uintptr_t>(interrupt_handler_68));
	setup_isr(69, reinterpret_cast<uintptr_t>(interrupt_handler_69));
	setup_isr(70, reinterpret_cast<uintptr_t>(interrupt_handler_70));
	setup_isr(71, reinterpret_cast<uintptr_t>(interrupt_handler_71));
	setup_isr(72, reinterpret_cast<uintptr_t>(interrupt_handler_72));
	setup_isr(73, reinterpret_cast<uintptr_t>(interrupt_handler_73));
	setup_isr(74, reinterpret_cast<uintptr_t>(interrupt_handler_74));
	setup_isr(75, reinterpret_cast<uintptr_t>(interrupt_handler_75));
	setup_isr(76, reinterpret_cast<uintptr_t>(interrupt_handler_76));
	setup_isr(77, reinterpret_cast<uintptr_t>(interrupt_handler_77));
	setup_isr(78, reinterpret_cast<uintptr_t>(interrupt_handler_78));
	setup_isr(79, reinterpret_cast<uintptr_t>(interrupt_handler_79));
	setup_isr(80, reinterpret_cast<uintptr_t>(interrupt_handler_80));
	setup_isr(81, reinterpret_cast<uintptr_t>(interrupt_handler_81));
	setup_isr(82, reinterpret_cast<uintptr_t>(interrupt_handler_82));
	setup_isr(83, reinterpret_cast<uintptr_t>(interrupt_handler_83));
	setup_isr(84, reinterpret_cast<uintptr_t>(interrupt_handler_84));
	setup_isr(85, reinterpret_cast<uintptr_t>(interrupt_handler_85));
	setup_isr(86, reinterpret_cast<uintptr_t>(interrupt_handler_86));
	setup_isr(87, reinterpret_cast<uintptr_t>(interrupt_handler_87));
	setup_isr(88, reinterpret_cast<uintptr_t>(interrupt_handler_88));
	setup_isr(89, reinterpret_cast<uintptr_t>(interrupt_handler_89));
	setup_isr(90, reinterpret_cast<uintptr_t>(interrupt_handler_90));
	setup_isr(91, reinterpret_cast<uintptr_t>(interrupt_handler_91));
	setup_isr(92, reinterpret_cast<uintptr_t>(interrupt_handler_92));
	setup_isr(93, reinterpret_cast<uintptr_t>(interrupt_handler_93));
	setup_isr(94, reinterpret_cast<uintptr_t>(interrupt_handler_94));
	setup_isr(95, reinterpret_cast<uintptr_t>(interrupt_handler_95));
	setup_isr(96, reinterpret_cast<uintptr_t>(interrupt_handler_96));
	setup_isr(97, reinterpret_cast<uintptr_t>(interrupt_handler_97));
	setup_isr(98, reinterpret_cast<uintptr_t>(interrupt_handler_98));
	setup_isr(99, reinterpret_cast<uintptr_t>(interrupt_handler_99));
	setup_isr(100, reinterpret_cast<uintptr_t>(interrupt_handler_100));
	setup_isr(101, reinterpret_cast<uintptr_t>(interrupt_handler_101));
	setup_isr(102, reinterpret_cast<uintptr_t>(interrupt_handler_102));
	setup_isr(103, reinterpret_cast<uintptr_t>(interrupt_handler_103));
	setup_isr(104, reinterpret_cast<uintptr_t>(interrupt_handler_104));
	setup_isr(105, reinterpret_cast<uintptr_t>(interrupt_handler_105));
	setup_isr(106, reinterpret_cast<uintptr_t>(interrupt_handler_106));
	setup_isr(107, reinterpret_cast<uintptr_t>(interrupt_handler_107));
	setup_isr(108, reinterpret_cast<uintptr_t>(interrupt_handler_108));
	setup_isr(109, reinterpret_cast<uintptr_t>(interrupt_handler_109));
	setup_isr(110, reinterpret_cast<uintptr_t>(interrupt_handler_110));
	setup_isr(111, reinterpret_cast<uintptr_t>(interrupt_handler_111));
	setup_isr(112, reinterpret_cast<uintptr_t>(interrupt_handler_112));
	setup_isr(113, reinterpret_cast<uintptr_t>(interrupt_handler_113));
	setup_isr(114, reinterpret_cast<uintptr_t>(interrupt_handler_114));
	setup_isr(115, reinterpret_cast<uintptr_t>(interrupt_handler_115));
	setup_isr(116, reinterpret_cast<uintptr_t>(interrupt_handler_116));
	setup_isr(117, reinterpret_cast<uintptr_t>(interrupt_handler_117));
	setup_isr(118, reinterpret_cast<uintptr_t>(interrupt_handler_118));
	setup_isr(119, reinterpret_cast<uintptr_t>(interrupt_handler_119));
	setup_isr(120, reinterpret_cast<uintptr_t>(interrupt_handler_120));
	setup_isr(121, reinterpret_cast<uintptr_t>(interrupt_handler_121));
	setup_isr(122, reinterpret_cast<uintptr_t>(interrupt_handler_122));
	setup_isr(123, reinterpret_cast<uintptr_t>(interrupt_handler_123));
	setup_isr(124, reinterpret_cast<uintptr_t>(interrupt_handler_124));
	setup_isr(125, reinterpret_cast<uintptr_t>(interrupt_handler_125));
	setup_isr(126, reinterpret_cast<uintptr_t>(interrupt_handler_126));
	setup_isr(127, reinterpret_cast<uintptr_t>(interrupt_handler_127));
	setup_isr(128, reinterpret_cast<uintptr_t>(interrupt_handler_128));
	setup_isr(129, reinterpret_cast<uintptr_t>(interrupt_handler_129));
	setup_isr(130, reinterpret_cast<uintptr_t>(interrupt_handler_130));
	setup_isr(131, reinterpret_cast<uintptr_t>(interrupt_handler_131));
	setup_isr(132, reinterpret_cast<uintptr_t>(interrupt_handler_132));
	setup_isr(133, reinterpret_cast<uintptr_t>(interrupt_handler_133));
	setup_isr(134, reinterpret_cast<uintptr_t>(interrupt_handler_134));
	setup_isr(135, reinterpret_cast<uintptr_t>(interrupt_handler_135));
	setup_isr(136, reinterpret_cast<uintptr_t>(interrupt_handler_136));
	setup_isr(137, reinterpret_cast<uintptr_t>(interrupt_handler_137));
	setup_isr(138, reinterpret_cast<uintptr_t>(interrupt_handler_138));
	setup_isr(139, reinterpret_cast<uintptr_t>(interrupt_handler_139));
	setup_isr(140, reinterpret_cast<uintptr_t>(interrupt_handler_140));
	setup_isr(141, reinterpret_cast<uintptr_t>(interrupt_handler_141));
	setup_isr(142, reinterpret_cast<uintptr_t>(interrupt_handler_142));
	setup_isr(143, reinterpret_cast<uintptr_t>(interrupt_handler_143));
	setup_isr(144, reinterpret_cast<uintptr_t>(interrupt_handler_144));
	setup_isr(145, reinterpret_cast<uintptr_t>(interrupt_handler_145));
	setup_isr(146, reinterpret_cast<uintptr_t>(interrupt_handler_146));
	setup_isr(147, reinterpret_cast<uintptr_t>(interrupt_handler_147));
	setup_isr(148, reinterpret_cast<uintptr_t>(interrupt_handler_148));
	setup_isr(149, reinterpret_cast<uintptr_t>(interrupt_handler_149));
	setup_isr(150, reinterpret_cast<uintptr_t>(interrupt_handler_150));
	setup_isr(151, reinterpret_cast<uintptr_t>(interrupt_handler_151));
	setup_isr(152, reinterpret_cast<uintptr_t>(interrupt_handler_152));
	setup_isr(153, reinterpret_cast<uintptr_t>(interrupt_handler_153));
	setup_isr(154, reinterpret_cast<uintptr_t>(interrupt_handler_154));
	setup_isr(155, reinterpret_cast<uintptr_t>(interrupt_handler_155));
	setup_isr(156, reinterpret_cast<uintptr_t>(interrupt_handler_156));
	setup_isr(157, reinterpret_cast<uintptr_t>(interrupt_handler_157));
	setup_isr(158, reinterpret_cast<uintptr_t>(interrupt_handler_158));
	setup_isr(159, reinterpret_cast<uintptr_t>(interrupt_handler_159));
	setup_isr(160, reinterpret_cast<uintptr_t>(interrupt_handler_160));
	setup_isr(161, reinterpret_cast<uintptr_t>(interrupt_handler_161));
	setup_isr(162, reinterpret_cast<uintptr_t>(interrupt_handler_162));
	setup_isr(163, reinterpret_cast<uintptr_t>(interrupt_handler_163));
	setup_isr(164, reinterpret_cast<uintptr_t>(interrupt_handler_164));
	setup_isr(165, reinterpret_cast<uintptr_t>(interrupt_handler_165));
	setup_isr(166, reinterpret_cast<uintptr_t>(interrupt_handler_166));
	setup_isr(167, reinterpret_cast<uintptr_t>(interrupt_handler_167));
	setup_isr(168, reinterpret_cast<uintptr_t>(interrupt_handler_168));
	setup_isr(169, reinterpret_cast<uintptr_t>(interrupt_handler_169));
	setup_isr(170, reinterpret_cast<uintptr_t>(interrupt_handler_170));
	setup_isr(171, reinterpret_cast<uintptr_t>(interrupt_handler_171));
	setup_isr(172, reinterpret_cast<uintptr_t>(interrupt_handler_172));
	setup_isr(173, reinterpret_cast<uintptr_t>(interrupt_handler_173));
	setup_isr(174, reinterpret_cast<uintptr_t>(interrupt_handler_174));
	setup_isr(175, reinterpret_cast<uintptr_t>(interrupt_handler_175));
	setup_isr(176, reinterpret_cast<uintptr_t>(interrupt_handler_176));
	setup_isr(177, reinterpret_cast<uintptr_t>(interrupt_handler_177));
	setup_isr(178, reinterpret_cast<uintptr_t>(interrupt_handler_178));
	setup_isr(179, reinterpret_cast<uintptr_t>(interrupt_handler_179));
	setup_isr(180, reinterpret_cast<uintptr_t>(interrupt_handler_180));
	setup_isr(181, reinterpret_cast<uintptr_t>(interrupt_handler_181));
	setup_isr(182, reinterpret_cast<uintptr_t>(interrupt_handler_182));
	setup_isr(183, reinterpret_cast<uintptr_t>(interrupt_handler_183));
	setup_isr(184, reinterpret_cast<uintptr_t>(interrupt_handler_184));
	setup_isr(185, reinterpret_cast<uintptr_t>(interrupt_handler_185));
	setup_isr(186, reinterpret_cast<uintptr_t>(interrupt_handler_186));
	setup_isr(187, reinterpret_cast<uintptr_t>(interrupt_handler_187));
	setup_isr(188, reinterpret_cast<uintptr_t>(interrupt_handler_188));
	setup_isr(189, reinterpret_cast<uintptr_t>(interrupt_handler_189));
	setup_isr(190, reinterpret_cast<uintptr_t>(interrupt_handler_190));
	setup_isr(191, reinterpret_cast<uintptr_t>(interrupt_handler_191));
	setup_isr(192, reinterpret_cast<uintptr_t>(interrupt_handler_192));
	setup_isr(193, reinterpret_cast<uintptr_t>(interrupt_handler_193));
	setup_isr(194, reinterpret_cast<uintptr_t>(interrupt_handler_194));
	setup_isr(195, reinterpret_cast<uintptr_t>(interrupt_handler_195));
	setup_isr(196, reinterpret_cast<uintptr_t>(interrupt_handler_196));
	setup_isr(197, reinterpret_cast<uintptr_t>(interrupt_handler_197));
	setup_isr(198, reinterpret_cast<uintptr_t>(interrupt_handler_198));
	setup_isr(199, reinterpret_cast<uintptr_t>(interrupt_handler_199));
	setup_isr(200, reinterpret_cast<uintptr_t>(interrupt_handler_200));
	setup_isr(201, reinterpret_cast<uintptr_t>(interrupt_handler_201));
	setup_isr(202, reinterpret_cast<uintptr_t>(interrupt_handler_202));
	setup_isr(203, reinterpret_cast<uintptr_t>(interrupt_handler_203));
	setup_isr(204, reinterpret_cast<uintptr_t>(interrupt_handler_204));
	setup_isr(205, reinterpret_cast<uintptr_t>(interrupt_handler_205));
	setup_isr(206, reinterpret_cast<uintptr_t>(interrupt_handler_206));
	setup_isr(207, reinterpret_cast<uintptr_t>(interrupt_handler_207));
	setup_isr(208, reinterpret_cast<uintptr_t>(interrupt_handler_208));
	setup_isr(209, reinterpret_cast<uintptr_t>(interrupt_handler_209));
	setup_isr(210, reinterpret_cast<uintptr_t>(interrupt_handler_210));
	setup_isr(211, reinterpret_cast<uintptr_t>(interrupt_handler_211));
	setup_isr(212, reinterpret_cast<uintptr_t>(interrupt_handler_212));
	setup_isr(213, reinterpret_cast<uintptr_t>(interrupt_handler_213));
	setup_isr(214, reinterpret_cast<uintptr_t>(interrupt_handler_214));
	setup_isr(215, reinterpret_cast<uintptr_t>(interrupt_handler_215));
	setup_isr(216, reinterpret_cast<uintptr_t>(interrupt_handler_216));
	setup_isr(217, reinterpret_cast<uintptr_t>(interrupt_handler_217));
	setup_isr(218, reinterpret_cast<uintptr_t>(interrupt_handler_218));
	setup_isr(219, reinterpret_cast<uintptr_t>(interrupt_handler_219));
	setup_isr(220, reinterpret_cast<uintptr_t>(interrupt_handler_220));
	setup_isr(221, reinterpret_cast<uintptr_t>(interrupt_handler_221));
	setup_isr(222, reinterpret_cast<uintptr_t>(interrupt_handler_222));
	setup_isr(223, reinterpret_cast<uintptr_t>(interrupt_handler_223));
	setup_isr(224, reinterpret_cast<uintptr_t>(interrupt_handler_224));
	setup_isr(225, reinterpret_cast<uintptr_t>(interrupt_handler_225));
	setup_isr(226, reinterpret_cast<uintptr_t>(interrupt_handler_226));
	setup_isr(227, reinterpret_cast<uintptr_t>(interrupt_handler_227));
	setup_isr(228, reinterpret_cast<uintptr_t>(interrupt_handler_228));
	setup_isr(229, reinterpret_cast<uintptr_t>(interrupt_handler_229));
	setup_isr(230, reinterpret_cast<uintptr_t>(interrupt_handler_230));
	setup_isr(231, reinterpret_cast<uintptr_t>(interrupt_handler_231));
	setup_isr(232, reinterpret_cast<uintptr_t>(interrupt_handler_232));
	setup_isr(233, reinterpret_cast<uintptr_t>(interrupt_handler_233));
	setup_isr(234, reinterpret_cast<uintptr_t>(interrupt_handler_234));
	setup_isr(235, reinterpret_cast<uintptr_t>(interrupt_handler_235));
	setup_isr(236, reinterpret_cast<uintptr_t>(interrupt_handler_236));
	setup_isr(237, reinterpret_cast<uintptr_t>(interrupt_handler_237));
	setup_isr(238, reinterpret_cast<uintptr_t>(interrupt_handler_238));
	setup_isr(239, reinterpret_cast<uintptr_t>(interrupt_handler_239));
	setup_isr(240, reinterpret_cast<uintptr_t>(interrupt_handler_240));
	setup_isr(241, reinterpret_cast<uintptr_t>(interrupt_handler_241));
	setup_isr(242, reinterpret_cast<uintptr_t>(interrupt_handler_242));
	setup_isr(243, reinterpret_cast<uintptr_t>(interrupt_handler_243));
	setup_isr(244, reinterpret_cast<uintptr_t>(interrupt_handler_244));
	setup_isr(245, reinterpret_cast<uintptr_t>(interrupt_handler_245));
	setup_isr(246, reinterpret_cast<uintptr_t>(interrupt_handler_246));
	setup_isr(247, reinterpret_cast<uintptr_t>(interrupt_handler_247));
	setup_isr(248, reinterpret_cast<uintptr_t>(interrupt_handler_248));
	setup_isr(249, reinterpret_cast<uintptr_t>(interrupt_handler_249));
	setup_isr(250, reinterpret_cast<uintptr_t>(interrupt_handler_250));
	setup_isr(251, reinterpret_cast<uintptr_t>(interrupt_handler_251));
	setup_isr(252, reinterpret_cast<uintptr_t>(interrupt_handler_252));
	setup_isr(253, reinterpret_cast<uintptr_t>(interrupt_handler_253));
	setup_isr(254, reinterpret_cast<uintptr_t>(interrupt_handler_254));
	setup_isr(255, reinterpret_cast<uintptr_t>(interrupt_handler_255));
}

void setup_isr(uint8_t intr_num, uintptr_t handler) {
	idt[intr_num].segment_offset_1 = handler & 0xFFFF;
	idt[intr_num].segment_offset_2 = handler >> 16;
	idt[intr_num].zero = 0;
	idt[intr_num].selector = 8; //code segment gdt offset
	idt[intr_num].type_attr = 0b10001111; //TODO: improve
}

void load_idt(void) {
	asm("lidt %0" :: "m" (idt_descriptor));
}

void test_idt(void) {
	asm("int $60");
	asm("int $90");
}