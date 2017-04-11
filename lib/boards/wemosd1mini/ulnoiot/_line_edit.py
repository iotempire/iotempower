# small line-editor
#
import os

def linecat(filename,frm=1,to=0):
    line=1
    for l in open(filename):
        if to > 0 and line>to:
            break;
        if line>=frm:
            print(line,l,end="")
        line += 1

def lineedit(filename,linenr):
    print("You want to edit line",linenr)
    linecat(filename,frm=linenr-2,to=linenr+2)
    print("Input new value for line %d:",linenr)
    newl=input()
    print("Replace")
    linecat(filename,frm=linenr,to=linenr)
    print("with")
    print(linenr,newl)
    print("Really replace? N/y")
    answer=input()
    answer=answer.lower()
    if answer=="y":
        newfilename=filename+".lineedit"
        output = open(newfilename,"wb")
        line = 1
        for l in open(filename):
            if line == linenr:
                output.write(newl+"\n")
            else:
                output.write(l)
            line+=1
        # allow append
        if line == linenr:
            output.write(newl+"\n")
    output.close()
    os.rename(newfilename,filename)