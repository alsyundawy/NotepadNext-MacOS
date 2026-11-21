local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"sol",
}

L.keywords = {
	[0] = "abstract address anonymous as assembly assert bool break byte bytes calldata catch constant constructor continue contract delete do else emit enum event external fallback false final fn for function global if immutable import indexed interface internal is library mapping memory modifier new null override payable pragma private public pure receive return returns revert string struct super this throw true try type typeof ufixed uint unchecked unicode using var view virtual while",
	[1] = "address bool string bytes byte int uint fixed ufixed mapping int8 int16 int32 int64 int128 int256 uint8 uint16 uint32 uint64 uint128 uint256 bytes1 bytes2 bytes3 bytes4 bytes8 bytes16 bytes32",
	[2] = "msg block tx abi blockhash gasleft keccak256 sha256 ripemd160 ecrecover addmod mulmod selfdestruct",
}

return L
