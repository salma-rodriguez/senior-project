DOC = slides

all: 
	pdflatex $(DOC)
	# bibtex $(DOC)
	pdflatex $(DOC)
	# pdflatex $(DOC)

clean:
	-rm *.pdf *.aux *.log *.ps *.dvi *.bbl *.blg *.out *.vrb *.nav *.snm *.toc

.PHONY: all clean
