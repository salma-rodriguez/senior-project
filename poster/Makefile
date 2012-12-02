DOC = poster

all: 
	pdflatex $(DOC)
	# bibtex $(DOC)
	pdflatex $(DOC)
	# pdflatex $(DOC)

clean:
	-rm *.aux *.log *.out *.nav *.snm *.toc *.pdf

.PHONY: all clean
