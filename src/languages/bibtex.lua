local L = {}

L.lexer = "bibtex"

L.singleLineComment = "% "

L.extensions = {
	"bib",
}

L.keywords = {
	[0] = "article book booklet inbook incollection inproceedings manual mastersthesis misc phdthesis proceedings techreport unpublished string comment preamble",
	[1] = "author title journal year volume number pages month note key publisher editor series address edition booktitle organization howpublished school institution",
}

return L
