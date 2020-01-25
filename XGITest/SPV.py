import sys

inputFileName = sys.argv[1]
resourceName = sys.argv[2]
outputFileName = inputFileName[:inputFileName.rfind('.')] + '.c'

inputFile = open(inputFileName, 'rb')
binary = inputFile.read().hex()
inputFile.close()

text = '\n'
text += 'static uint32_t SPV_' + resourceName + '[] =\n{\n\t'
i = 0
while i < len(binary):
	word = binary[i:i + 8]
	word = word[6:8] + word[4:6] + word[2:4] + word[0:2]
	text += '0x' + word + ','
	i += 8
	if i % 64 == 0: text += '\n\t'

text += '\n};\n'

outputFile = open(outputFileName, 'w')
outputFile.write(text)
outputFile.close()
