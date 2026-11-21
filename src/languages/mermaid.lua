local L = {}

L.lexer = "markdown"

L.singleLineComment = "%% "

L.extensions = {
	"mermaid",
	"mmd",
}

L.keywords = {
	[0] = "graph flowchart sequenceDiagram classDiagram stateDiagram erDiagram gantt pie gitGraph quadrantChart xyChart requirementDiagram architecture-beta mindmap timeline C4Context subGraph end click style linkStyle classDef class participant actor loop alt else opt par and rect critical end",
}

return L
