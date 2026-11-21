local L = {}

L.lexer = "bash"

L.singleLineComment = "# "

L.extensions = {
	"dockerfile",
	"containerfile",
}

L.keywords = {
	[0] = "FROM MAINTAINER RUN CMD LABEL EXPOSE ENV ADD COPY ENTRYPOINT VOLUME USER WORKDIR ARG ONBUILD STOPSIGNAL HEALTHCHECK SHELL AS as from run cmd copy add expose env workdir user entrypoint",
}

return L
