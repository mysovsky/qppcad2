#include <iostream>
#include <QApplication>
#include <QStyleFactory>
#include <QFontDatabase>
#include <qppcad/ui/main_window.hpp>
#include <qppcad/core/app_state.hpp>
#include <qppcad/core/hotkey_manager.hpp>
#include <QMainWindow>
#include <QTextStream>
#include <QCommandLineParser>
#include <QString>
#include <QMessageBox>
#include <pybind11/embed.h>

using namespace qpp;
using namespace qpp::cad;

int main (int argc, char **argv){   //, char **envp) {
  try {
	  std::ios_base::sync_with_stdio(false);

	  QCoreApplication::setOrganizationName("igc");
	  QCoreApplication::setOrganizationDomain("igc.irk.ru");
	  QCoreApplication::setApplicationName("qppcad");
	  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	  QApplication app(argc, argv);

	  QCommandLineParser parser;
	  parser.setApplicationDescription("qpp::cad");
	  parser.addHelpOption();
	  parser.addVersionOption();
	  parser.addPositionalArgument("file", "The file to open.");

	  QCommandLineOption target_fmt_option(QStringList() << "f" << "format",
										   QCoreApplication::translate("main", "Force file format"),
										   QCoreApplication::translate("main", "file_format"));
	  parser.addOption(target_fmt_option);

	  parser.process(QCoreApplication::arguments());
	  const QStringList args = parser.positionalArguments();

	  app_state_t::init_inst();

	  app_state_t *astate = app_state_t::get_inst();

	  astate->init_managers();
	  astate->ws_mgr->init_ws_item_bhv_mgr();
	  astate->load_settings();
	  astate->init_fixtures();
	  astate->ws_mgr->init_default();

	  if (!args.empty()) {

		  std::string file_format = "";

		  if (parser.isSet(target_fmt_option))
			file_format = parser.value(target_fmt_option).toStdString();

		  for (auto &rec : args) {
			  astate->ws_mgr->load_from_file_autodeduce(rec.toStdString(), file_format);
			}

		} else {

		  if (parser.isSet(target_fmt_option)) {
			  return 0;
			}

		}

	  QSurfaceFormat format;
	  format.setDepthBufferSize(24);

	  format.setSamples(astate->m_num_samples);
	  format.setVersion(3, 3);
	  format.setProfile(QSurfaceFormat::CoreProfile);
	  QSurfaceFormat::setDefaultFormat(format);

	  qApp->setStyle(QStyleFactory::create("Fusion"));
	  astate->init_styles();
	  qApp->setPalette(astate->m_app_palette);

	  std::vector<int> fnt_id = {
		QFontDatabase::addApplicationFont("://fonts/Open_Sans/OpenSans-Light.ttf"),
		QFontDatabase::addApplicationFont("://fonts/Open_Sans/OpenSans-Regular.ttf"),
		QFontDatabase::addApplicationFont("://fonts/Open_Sans/OpenSans-Bold.ttf"),
	  };

	  QString family = QFontDatabase::applicationFontFamilies(fnt_id[0]).at(0);
	  astate->m_font_name = family;

	  QIcon icon("://images/icon.svg");
	  app.setWindowIcon(icon);

	  QFile file("://style.qss");
	  file.open(QFile::ReadOnly);
	  QString style_sheet = QLatin1String(file.readAll());
	  app.setStyleSheet(style_sheet);

	  astate -> plug_mgr = std::make_unique<plugin_manager_t>(astate -> m_plugins_dir);
	  astate -> plug_mgr -> init();

	  main_window_t w;
	  astate->hotkey_mgr->m_main_window = &w;
	  astate->hotkey_mgr->bootstrap_from_restore_info();

	  w.rebuild_recent_files_menu();
	  astate->ws_mgr->m_bhv_mgr->cache_obj_insp_widgets();
	  w.showMaximized();
	  int ret_code = app.exec();

	  app_state_t::get_inst()->save_settings();

	  return ret_code;
	} catch (const py::error_already_set &e) {
        std::cerr << "Caught pybind11::error_already_set in main: " << e.what() << std::endl;
        if (e.trace()) {
            py::print(e.trace());
        }
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in main: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Caught unknown exception in main" << std::endl;
        return 1;
    }
    return 0;
}
