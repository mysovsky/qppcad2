#include <QCoreApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QPainter>
#include <QString>
#include <QTableWidgetItem>
#include <qppcad/core/app_state.hpp>
#include <qppcad/python/plugin_param_editor.hpp>

using namespace qpp;
using namespace qpp::cad;

int plugin_param_model_t::rowCount(const QModelIndex &parent) const {
  if (plugin)
    return plugin->params.size();
  else
    return 0;
}

int plugin_param_model_t::columnCount(const QModelIndex &parent) const {
  return 3;
}

QVariant plugin_param_model_t::data(const QModelIndex &index, int role) const {

  if (!plugin)
    return QVariant();

  if (role == Qt::TextAlignmentRole) {
    return Qt::AlignCenter;
  }

  int i = index.row();
  int j = index.column();

  if (role == Qt::DisplayRole || role == Qt::EditRole) {

    switch (j) {

    case 0:
      return QString(plugin->params[i]->name.c_str());
      break;

    case 1:
      return QString(type_data::type_name[plugin->params[i]->type].c_str());
      break;

    case 2:
      if (plugin->params[i]->type == type_bool)
        return QVariant();
      else
        return QString(plugin->params[i]->sval.c_str());
      break;

    default:
      return QVariant();
      break;
    }
  } else if (j == 2 && plugin->params[i]->type == type_bool &&
             role == Qt::CheckStateRole) {
    if (std::get<bool>(plugin->params[i]->value))
      return Qt::Checked;
    else
      return Qt::Unchecked;
  }
  return QVariant();
}

QVariant plugin_param_model_t::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const {

  if (role == Qt::DisplayRole) {

    if (orientation == Qt::Horizontal) {
      switch (section) {

      case 0:
        return tr("Parameter");
        break;

      case 1:
        return tr("Type");
        break;

      case 2:
        return tr("Value");
        break;

      default:
        return QString("");
      }
    }
  }
  return QVariant();
}

Qt::ItemFlags plugin_param_model_t::flags(const QModelIndex &index) const {

  Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled;
  if (index.column() == 2) {
    if (plugin->params[index.row()]->type == type_bool)
      flags = flags | Qt::ItemFlag::ItemIsUserCheckable;
    else
      flags = flags | Qt::ItemFlag::ItemIsEditable;
  }
  return flags;
}

bool plugin_param_model_t::setData(const QModelIndex &index,
                                   const QVariant &value, int role) {
  int i = index.row();
  int j = index.column();

  if (j == 2 && role == Qt::EditRole) {

    if (value.type() == QVariant::String) {

      plugin->params[i]->sval = value.toString().toStdString();
      return true;
    }
  }

  if (role == Qt::CheckStateRole && j == 2 &&
      plugin->params[i]->type == type_bool) {
    if ((Qt::CheckState)value.toInt() == Qt::Checked) {
      // user has checked item
      plugin->params[i]->value = true;
      plugin->params[i]->sval = "true";
      return true;
    } else {
      // user has unchecked item
      plugin->params[i]->value = false;
      plugin->params[i]->sval = "false";
      return true;
    }
  }
  return QAbstractTableModel::setData(index, value, role);
}

void plugin_param_model_t::bind(std::shared_ptr<python_plugin_t> p) {
  plugin = p;
  for (auto prm : plugin->params)
    prm->sval = prm->default_sval;
}

void plugin_param_model_t::unbind() { plugin = nullptr; }

std::vector<std::shared_ptr<ws_item_t>> plugin_param_editor_t::list_ws_items() {
  app_state_t *astate = app_state_t::get_inst();
  auto cur_ws = astate->ws_mgr->get_cur_ws();
  std::vector<std::shared_ptr<ws_item_t>> list;
  for (auto it : cur_ws->m_ws_items) {
    list.push_back(it);
  }
  return cur_ws->m_ws_items;
}
colorTableCell::colorTableCell(QTableView *_ptbl, const QModelIndex &_I,
                               plugin_param_t *_prm)
    : QWidget(_ptbl->viewport()) {
  param_tbl = _ptbl;
  I = _I;
  color_param = _prm;
}

void colorTableCell::setColor(QColor col) {
  color = col;
  update();
  vector3<float> cl(color.redF(), color.greenF(), color.blueF());
  color_param->value = cl;
}

void colorTableCell::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(rect(), color);
}

void colorTableCell::mouseReleaseEvent(QMouseEvent *event) {
  QColor newcolor = QColorDialog::getColor(color);
  app_state_t *astate = app_state_t::get_inst();
  if (newcolor.isValid()) {
    astate->tlog("color valid {} {} {}", color.redF(), color.greenF(),
                 color.blueF());
    color = newcolor;
    setColor(newcolor);
  } else {
    astate->tlog("color invalid");
  }
}

plugin_param_editor_t::plugin_param_editor_t(
    std::shared_ptr<python_plugin_t> p) {

  plugin = p;
  app_state_t *astate = app_state_t::get_inst();
  astate->tlog("plug param editor ");
  setWindowTitle(tr(plugin->plug_name.c_str()));

  main_lt = new QVBoxLayout;
  button_lt = new QHBoxLayout;

  ok_button = new QPushButton(tr("Ok"));
  ok_button->setFixedWidth(astate->size_guide.common_button_fixed_w());
  cancel_button = new QPushButton(tr("Cancel"));
  cancel_button->setFixedWidth(astate->size_guide.common_button_fixed_w());

  button_lt->addStretch();
  button_lt->addWidget(ok_button, 0, Qt::AlignCenter);
  button_lt->addWidget(cancel_button, 0, Qt::AlignCenter);
  button_lt->addStretch();

  param_tbl = new QTableView;

  param_mdl = new plugin_param_model_t;
  param_mdl->bind(plugin);
  param_tbl->setModel(param_mdl);

  param_tbl->setFixedWidth(650);
  param_tbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  param_tbl->verticalHeader()->hide();
  param_tbl->horizontalHeader()->setStretchLastSection(true);

  for (int i = 0; i < plugin->params.size(); i++)
    astate->tlog("param {} type {}", plugin->params[i]->name,
                 plugin->params[i]->type);
  auto ws_items = list_ws_items();
  for (auto g : ws_items) {
    astate->tlog("cmb_itm {}", (g->m_name.c_str()));
  }

  for (int i = 0; i < plugin->params.size(); i++)
    if (plugin->params[i]->type == type_qpp_geometry) {
      auto cmb_itm = new QComboBox;

      astate->tlog("n ws items={}", ws_items.size());
      for (auto g : ws_items) {
        auto gv = std::static_pointer_cast<geom_view_t>(g);
        if (!gv || !gv->m_geom)
          continue;
        cmb_itm->addItem(tr(g->m_name.c_str()));
        astate->tlog("cmb_itm {}", (g->m_name.c_str()));
      }

      QObject::connect(
          cmb_itm,
          static_cast<void (QComboBox::*)(int)>(
              &QComboBox::currentIndexChanged),
          [this, i, ws_items, astate](int idx) {
            this->plugin->params[i]->value =
                std::static_pointer_cast<geom_view_t>(ws_items[idx])->m_geom;
            astate->tlog("plug param edit combo : {}",
                         std::get<std::shared_ptr<xgeometry<float>>>(
                             this->plugin->params[i]->value)
                             ->nat());
          });
      plugin->params[i]->value =
          std::static_pointer_cast<geom_view_t>(ws_items[0])->m_geom;

      auto I = param_mdl->index(i, 2);
      param_tbl->setIndexWidget(I, cmb_itm);
    } else if (plugin->params[i]->type == type_qpp_atom_vectors) {
      auto cmb_itm = new QComboBox;

      astate->tlog("n ws items={}", ws_items.size());
      for (auto g : ws_items) {
        auto gv = std::static_pointer_cast<arrow_array_view_t>(g);
        if (!gv || !gv->m_binded_vectors)
          continue;
        cmb_itm->addItem(tr(g->m_name.c_str()));
        astate->tlog("cmb_itm {}", (g->m_name.c_str()));
      }

      QObject::connect(
          cmb_itm,
          static_cast<void (QComboBox::*)(int)>(
              &QComboBox::currentIndexChanged),
          [this, i, ws_items, astate](int idx) {
            this->plugin->params[i]->value =
                std::static_pointer_cast<arrow_array_view_t>(ws_items[idx])
                    ->m_binded_vectors;
            astate->tlog("plug param edit combo : {}",
                         std::get<std::shared_ptr<geom_atom_vectors<float>>>(
                             this->plugin->params[i]->value)
                             ->vectors.size());
          });
      plugin->params[i]->value =
          std::static_pointer_cast<geom_view_t>(ws_items[0])->m_geom;

      auto I = param_mdl->index(i, 2);
      param_tbl->setIndexWidget(I, cmb_itm);
    } else if (plugin->params[i]->type == basic_types::type_qpp_color) {
      // Color selection button
      QString bt_clr_str = "";
      vector3<float> btc(1.0, 0.0, 1.0);
      QColor color_bck(btc[0], btc[1], btc[2]);
      auto I = param_mdl->index(i, 2);
      auto cll = new colorTableCell(param_tbl, I, plugin->params[i].get());
      cll->setColor(color_bck);
      param_tbl->setIndexWidget(I, cll);
    } else if (plugin->params[i]->choice.size() > 0) {
      auto cmb_itm = new QComboBox;

      astate->tlog("n ws items={}", ws_items.size());
      for (auto c : plugin->params[i]->choice) {
        cmb_itm->addItem(tr(c.c_str()));
        astate->tlog("cmb_itm {}", c.c_str());
      }

      QObject::connect(cmb_itm,
                       static_cast<void (QComboBox::*)(int)>(
                           &QComboBox::currentIndexChanged),
                       [this, i, ws_items, astate](int idx) {
                         this->plugin->params[i]->value = idx;
                         this->plugin->params[i]->sval = t2s(idx);
                         astate->tlog("plug param edit combo : {}", idx);
                       });
      plugin->params[i]->fromString(plugin->params[i]->default_sval);

      auto I = param_mdl->index(i, 2);
      param_tbl->setIndexWidget(I, cmb_itm);
    }

  ifile = -1;
  for (int ii = 0; ii < plugin->params.size(); ii++)
    if (plugin->params[ii]->browse != "") {
      ifile = ii;
      break;
    }

  auto descr = new qspoiler_widget_t(tr("Description"), nullptr, true);
  auto descr_lt = new QVBoxLayout;
  descr_lt->addWidget(new QLabel(tr(plugin->description.c_str())));
  descr->add_content_layout(descr_lt);
  descr->setFixedWidth(650);

  main_lt->addWidget(descr);
  main_lt->addWidget(param_tbl);

  if (ifile >= 0) {
    browse_btn = new QPushButton(tr("Browse"));
    browse_btn->setFixedWidth(astate->size_guide.common_button_fixed_w());
    auto browse_lt = new QHBoxLayout;
    std::string br_label = "Use this button to browse the file \"";
    br_label = br_label + plugin->params[ifile]->name + "\"";
    browse_lt->addWidget(browse_btn);
    browse_lt->addWidget(new QLabel(tr(br_label.c_str())));
    main_lt->addLayout(browse_lt);
  }

  main_lt->addLayout(button_lt);
  setLayout(main_lt);

  connect(ok_button, &QPushButton::clicked, this,
          &plugin_param_editor_t::ok_button_clicked);

  connect(cancel_button, &QPushButton::clicked, this,
          &plugin_param_editor_t::cancel_button_clicked);

  if (ifile >= 0)
    connect(browse_btn, &QPushButton::clicked, this,
            &plugin_param_editor_t::browse_button_clicked);
}

void plugin_param_editor_t::ok_button_clicked() {
  param_mdl->unbind();
  accept();
}

void plugin_param_editor_t::cancel_button_clicked() {
  param_mdl->unbind();
  reject();
}

void plugin_param_editor_t::browse_button_clicked() {
  std::string s;
  if (plugin->params[ifile]->browse == "save")
    s = QFileDialog::getSaveFileName(this, "Select file",
                                     plugin->params[ifile]->sval.c_str(), "*")
            .toStdString();
  else if (plugin->params[ifile]->browse == "open")
    s = QFileDialog::getOpenFileName(this, "Select file",
                                     plugin->params[ifile]->sval.c_str(), "*")
            .toStdString();
  plugin->params[ifile]->sval = s;
}
