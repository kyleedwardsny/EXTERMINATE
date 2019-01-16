#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>
#include <QByteArray>
#include <QCoreApplication>
#include <QIODevice>
#include <QVector>

#include <QtMath>

#include <QDebug>

class Dalek : public QIODevice
{
  Q_OBJECT

public:
  Dalek(QAudioOutput* output, QObject* parent = nullptr);

signals:
  void dataReady(QByteArray const& data);

protected:
  qint64 readData(char* data, qint64 maxlen) override;

  qint64 writeData(char const* data, qint64 len) override;

private slots:
  void sendData(QByteArray const& data);

private:
  QAudioOutput* output;
  QIODevice* outputDevice = nullptr;
  int state = 0;
};

Dalek::Dalek(QAudioOutput* output, QObject* parent)
  : QIODevice{parent}
  , output{output}
{
  connect(this, &Dalek::dataReady, this, &Dalek::sendData, Qt::QueuedConnection);
}

qint64 Dalek::readData(char* data, qint64 maxlen)
{
  return 0;
}

qint64 Dalek::writeData(char const* data, qint64 len)
{
  int num = len / sizeof(qint16);
  auto input = reinterpret_cast<qint16 const*>(data);
  QVector<qint16> buf(num);

  for (int i = 0; i < num; i++) {
    buf[i] = input[i] * qSin(state * 30.0 * 2.0 * M_PI / 48000.0);
    if (++state >= 48000) {
      state -= 48000;
    }
  }

  emit this->dataReady(QByteArray(reinterpret_cast<char const*>(buf.data()), len));
  return len;
}

void Dalek::sendData(QByteArray const& data)
{
  if (!this->outputDevice) {
    this->outputDevice = this->output->start();
  }
  this->outputDevice->write(data);
}

int main(int argc, char** argv)
{
  QCoreApplication app{argc, argv};

  QAudioFormat fmt;
  fmt.setChannelCount(1);
  fmt.setSampleRate(48000);
  fmt.setSampleSize(16);
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setCodec("audio/pcm");

  QAudioInput input{fmt};
  QAudioOutput output{fmt};

  input.setBufferSize(1024);
  output.setBufferSize(1024);

  Dalek caan{&output};
  caan.open(QIODevice::WriteOnly);
  input.start(&caan);

  return app.exec();
}

#include "EXTERMINATE.moc"
