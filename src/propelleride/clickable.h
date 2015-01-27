#include <QLabel>
#include <QMouseEvent>
#include <QString>

class ClickableLabel : public QLabel
{
    Q_OBJECT
  
public:
    explicit ClickableLabel(QWidget * parent = 0 );
    ~ClickableLabel();
           
signals:
    void clicked();

protected:
    void mousePressEvent ( QMouseEvent * event ) ;
};

