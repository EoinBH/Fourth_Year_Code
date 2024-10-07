import stanza
import sys
print('sys.argv: ', sys.argv)
stanza.download('en') # download English model
nlp = stanza.Pipeline('en') # initialize English neural pipeline
sentence = ''
first_arg = sys.argv[0]
print('First argument: ', first_arg)
last_arg = sys.argv[len(sys.argv) - 1]
print('Last argument: ', last_arg)
for word in sys.argv:
    if word != first_arg:
        if word != last_arg:
            sentence += word + ' '
        else:
            sentence += word
print('Final sentence: ', sentence)
doc = nlp(sentence) # run annotation over a sentence
# doc = nlp("Beidh sé go hiontach ár dteanga dhúchais a bheith á cur os comhair an tsaoil mhóir oíche na nOscar")
# print(doc)
# print(doc.entities)
max_depth = 0
curr_depth = 0
for sentence in doc.sentences:
    tree = sentence.constituency
    #print(tree)
    #for element in range(0, len(tree)):

        #if tree[element] == '(':
        #    curr_depth += 1
        #elif tree[element] == ')':
        #    curr_depth -= 1
    #print("Tree depth: ", tree.children[0].depth, "\n")
    for child in tree.children:
        curr_depth = child.depth()
        #print("Current depth: ", curr_depth, "\n")
        max_depth = max(max_depth, curr_depth)

    print("Max depth: ", max_depth)
