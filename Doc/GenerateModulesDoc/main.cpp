// To the simplest :
// g++ -o gen main.cpp -I../../GLIP-Lib/include -L../../GLIP-Lib/lib -lGL -lglip
#include <GLIPLib.hpp>

int main(void)
{	
	try
	{
		std::string str = "## Modules List\n";

		std::vector<Glip::Modules::LayoutLoaderModule*> v = Glip::Modules::LayoutLoaderModule::getBasicModulesList();
		for(std::vector<Glip::Modules::LayoutLoaderModule*>::const_iterator it=v.begin(); it!=v.end(); it++)
		{
			Glip::Modules::LayoutLoaderModule& m = *(*it);
			const std::vector<std::pair<std::string,std::string> > args = m.getArgumentsDescriptions();
			str += "### " + m.getName() + "\n";
			str += "<blockquote>\n";
			str += "<b>CALL</b>:" + m.getName();
			if(m.getMinNumArguments()==0 && m.getMaxNumArguments()>0)
				str += "[";
			if(m.getMinNumArguments()>0 || m.getMaxNumArguments()>0)
				str += "(";
			for(std::vector<std::pair<std::string,std::string> >::const_iterator itArg=args.begin(); itArg!=args.end();)
			{
				str += itArg->first;
				itArg++;
				if(itArg!=args.end())
				{
					if(std::distance(args.begin(),itArg)==m.getMinNumArguments())
						str += " [";
					str += ", ";
				}
				else
				{
					if(m.getMinNumArguments()>0 && m.getMinNumArguments()<args.size())
						str += "]";
					str += ")";
				}
			}
			str += "<br>\n";
			if(m.bodyPresenceTest()>=0)
			{
				if(m.bodyPresenceTest()==0)
					str += "[";
				str += "{<br>\n";
				str += "&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>\n";
				str += "}";
				if(m.bodyPresenceTest()==0)
					str += "]";
				str += "<br>\n";
			}
			str += "</blockquote>\n";

			str += "\n";
			str += m.getDescription();
			str += "\n\n";

			if(m.getMinNumArguments()>=0 || m.getMaxNumArguments()>=0)
			{
				str += "<table class=\"glipDescrTable\">\n";
				str += "<tr class=\"glipDescrHeaderRow\"><th class=\"glipDescrHeaderFirstColumn\">Argument</th><th>Description</th></tr>\n";
				for(std::vector<std::pair<std::string,std::string> >::const_iterator itArg=args.begin(); itArg!=args.end(); itArg++)
					str += "<tr class=\"glipDescrRow\"><td><i>" + itArg->first + "</i></td> <td>" + itArg->second + "</td></tr>\n";
				str += "</table>\n\n";
			}
			
			if(m.bodyPresenceTest()>=0)
				str += "<b>Body</b> : " + m.getBodyDescription() + "\n\n";
		}

		// Print :
		std::cout << str << std::endl;
	}
	catch(Glip::Exception& e)
	{
		std::cerr << "Exception caught : " << std::endl;
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
