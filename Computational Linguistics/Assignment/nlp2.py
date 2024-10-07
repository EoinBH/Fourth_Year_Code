import stanza
stanza.download('en') # download English model
nlp = stanza.Pipeline('en') # initialize English neural pipeline
f = open("Twitter_all_sentences.txt", "r")
overall_depth = 0
num_sentences = 0
overall_max_depth = 0
line_index = 1
total_lines = 3211 #Dail = 3548, Twitter = 3211
overall_max_depth_sentence = ""
file_name = "./Output/Twitter-Trees-Depth.csv"
newfile = open(file_name, "x")
newfile.write("Tree,Max-Depth\n")
for sentence in f:
    percent_complete = 100*(line_index / total_lines)
    print("Processing line", line_index, "of", total_lines, "| Percentage complete:", percent_complete, "%")
    doc = nlp(sentence) # run annotation over a sentence
    max_depth = 0
    curr_depth = 0
    for sentence in doc.sentences:
        tree = sentence.constituency
        newfile.write(str(tree) + ",")
        for child in tree.children:
            curr_depth = child.depth()
            max_depth = max(max_depth, curr_depth)
            overall_max_depth = max(overall_max_depth, max_depth)
            if curr_depth == overall_max_depth:
                overall_max_depth_sentence = sentence
        newfile.write(str(max_depth) + "\n")
        overall_depth += max_depth
        num_sentences += 1
    line_index += 1
overall_depth_avg = overall_depth / num_sentences
print("Number of sentences processed = ", num_sentences)
print("Average syntax tree depth = ", overall_depth_avg)
print("Maximum tree depth encountered = ", overall_max_depth)
print("Sentence in question = ", overall_max_depth_sentence)
print("Tree of sentence in question = ", overall_max_depth_sentence.constituency)
newfile.close()    