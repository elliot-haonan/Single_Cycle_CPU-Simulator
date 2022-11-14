int bits_to_int(bitset<32>* bits)
{
	if(((*bits).to_string()).substr(0,1) == "0") // positive
		return (*bits).to_ulong();
	else // negative
		return -(((*bits).flip()).to_ulong() + 1);

}