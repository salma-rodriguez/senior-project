DOC = slides

all: 
	pdflatex $(DOC)
	# bibtex $(DOC)
	pdflatex $(DOC)
	# pdflatex $(DOC)

clean:
	-rm *.pdf *.aux *.log *.out *.nav *.snm *.toc

.PHONY: all clean
