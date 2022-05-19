# to include the font data and various other parts
# without converting them to header files

# Set section address range and flags, section contains data
	.section .text.wrapper, "x", @progbits
# Store current settings
	.set push
# Don't reorder data/instructions?
	.set noreorder

# Declare object(s) global
	.global cursor_data
	.global font_data


	.align 4 # Advance location counter to next word-alignment

cursor_data:
		.incbin "../../assets/CURSOR.TIM"
		.align 4 # Advance location counter to next word-alignment

font_data:
		.incbin "../../assets/8X8FONT.TIM"
		.align 4 # Advance location counter to next word-alignment


# Restore previous settings
	.set pop


