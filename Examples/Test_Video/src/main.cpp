#include "VideoPlayer.hpp"

int main(int argc, char** argv)
{
	/*QApplication app(argc, argv);
	app.setApplicationName("testVideo");

	QStringList types = Phonon::BackendCapabilities::availableMimeTypes();
	for(int i = 0; i<types.size(); i++)
		std::cout << types[i].toStdString() << std::endl;

	Phonon::VideoPlayer *player = new Phonon::VideoPlayer(Phonon::VideoCategory);
	//Phonon::MediaSource src("/home/arkh/Images/Wallpapers/.template/Yoko/perfect.webm");
	Phonon::MediaSource src("/home/arkh/Images/Wallpapers/.template/Yoko/electroSummerWetRepublic.mp4");
	player->show();
	player->play(src);
	player->play();

	return app.exec();*/

	VideoModApplication app(argc, argv);
	return app.exec();
}
